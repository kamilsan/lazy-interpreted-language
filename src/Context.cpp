#include "Context.hpp"


#include "Executor.hpp"

RuntimeVariableAnalyser::RuntimeVariableAnalyser():
  symbolValid_(false), type_(), value_(nullptr), context_() {}

void RuntimeVariableAnalyser::visit(RuntimeVariableSymbol& symbol)
{
  type_ = symbol.getType();
  value_ = symbol.getValue();
  context_ = symbol.getContext();
  symbolValid_ = true;
}

void RuntimeVariableAnalyser::visit(RuntimeFunctionSymbol&)
{
  symbolValid_ = false;
}

RuntimeFunctionAnalyser::RuntimeFunctionAnalyser():
  symbolValid_(false), returnType_(), arguments_(), body_(nullptr) {}

void RuntimeFunctionAnalyser::visit(RuntimeVariableSymbol& node)
{
  symbolValid_ = node.getType() == TypeName::Function;
}

void RuntimeFunctionAnalyser::visit(RuntimeFunctionSymbol& symbol)
{
  symbolValid_ = true;
  arguments_ = symbol.getArguments();
  returnType_ = symbol.getReturnType();
  body_ = symbol.getBody();
}

void ValueChanger::visit(RuntimeVariableSymbol& symbol)
{
  symbol.setValue(value_);
}

void ValueChanger::visit(RuntimeFunctionSymbol&)
{

}

Context::Context(): scopes_()
{
  auto globalScope = std::unordered_map<std::string, std::shared_ptr<RuntimeSymbol>>();
  scopes_.push_back(std::move(globalScope));
}

void Context::enterScope()
{
  auto localScope = std::unordered_map<std::string, std::shared_ptr<RuntimeSymbol>>();
  scopes_.push_back(std::move(localScope));
}

void Context::leaveScope()
{
  scopes_.pop_back();
}

void Context::addSymbol(const std::string& name, std::shared_ptr<RuntimeSymbol> symbol)
{
  scopes_.back().insert(std::pair<std::string, std::shared_ptr<RuntimeSymbol>>{name, std::move(symbol)});
}

#include <iostream>

void Context::debug() const
{
  int depth = scopes_.size();
  for(auto scopesIt = scopes_.crbegin(); scopesIt != scopes_.crend(); ++scopesIt, --depth)
  {
    std::cout << "Symbols in scope " << depth << "\n";
    for(const auto& symbol : *scopesIt)
    {
      const auto name = symbol.first;
      const auto sym = symbol.second;

      RuntimeVariableAnalyser vis{};
      sym->accept(vis);
      if(vis.isSymbolValid())
      {
        Executor executor{vis.getContext()};
        vis.getValue()->accept(executor);
        std::cout << "Variable " << name << " with value " << std::get<double>(executor.getValue()) << "\n";
      }
      else
        std::cout << "Function " << name << "\n";
    }
  }
}

Context Context::clone() const
{
  Context newContext{};
  for(auto scopesIt = scopes_.cbegin(); scopesIt != scopes_.cend(); ++scopesIt)
  {
    for(const auto& symbol : *scopesIt)
    {
      newContext.addSymbol(symbol.first, symbol.second);
    }

    if(scopesIt + 1 != scopes_.cend())
      newContext.enterScope();
  }
  return newContext;
}

std::optional<std::reference_wrapper<RuntimeSymbol>> Context::lookup(const std::string& name, int maxDepth) const
{
  int depth = 1;
  for(auto scopesIt = scopes_.crbegin(); scopesIt != scopes_.crend(); ++scopesIt, ++depth)
  {
    auto it = scopesIt->find(name);
    if(it != scopesIt->end())
      return *it->second;

    if(maxDepth != 0 && depth == maxDepth)
      break;
  }

  return {};  
}
