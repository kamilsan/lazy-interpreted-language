#include "Context.hpp"


RuntimeVariableAnalyser::RuntimeVariableAnalyser():
  symbolValid_(false), type_(), value_(nullptr) {}

void RuntimeVariableAnalyser::visit(RuntimeVariableSymbol& symbol)
{
  type_ = symbol.getType();
  value_ = symbol.getValue();
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


Context::Context(): scopes_()
{
  auto globalScope = std::unordered_map<std::string, std::unique_ptr<RuntimeSymbol>>();
  scopes_.push_back(std::move(globalScope));
}

void Context::enterScope()
{
  auto localScope = std::unordered_map<std::string, std::unique_ptr<RuntimeSymbol>>();
  scopes_.push_back(std::move(localScope));
}

void Context::leaveScope()
{
  scopes_.pop_back();
}

void Context::addSymbol(const std::string& name, std::unique_ptr<RuntimeSymbol> symbol)
{
  scopes_.back().insert(std::pair<std::string, std::unique_ptr<RuntimeSymbol>>{name, std::move(symbol)});
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
