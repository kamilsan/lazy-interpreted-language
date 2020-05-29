#include "Symbol.hpp"


VariableAnalyser::VariableAnalyser():
  symbolValid_(false), type_() {}

void VariableAnalyser::visit(VariableSymbol& symbol)
{
  type_ = symbol.getType();
  symbolValid_ = true;
}

void VariableAnalyser::visit(FunctionSymbol&)
{
  symbolValid_ = false;
}

FunctionAnalyser::FunctionAnalyser():
  symbolValid_(false), returnType_(), arguments_() {}

void FunctionAnalyser::visit(VariableSymbol& node)
{
  symbolValid_ = node.getType() == TypeName::Function;
}

void FunctionAnalyser::visit(FunctionSymbol& symbol)
{
  symbolValid_ = true;
  arguments_ = symbol.getArguments();
  returnType_ = symbol.getReturnType();
}


SymbolTable::SymbolTable(): scopes_()
{
  auto globalScope = std::unordered_map<std::string, std::unique_ptr<Symbol>>();
  scopes_.push_back(std::move(globalScope));
}

void SymbolTable::enterScope()
{
  auto localScope = std::unordered_map<std::string, std::unique_ptr<Symbol>>();
  scopes_.push_back(std::move(localScope));
}

void SymbolTable::leaveScope()
{
  scopes_.pop_back();
}

void SymbolTable::addSymbol(const std::string& name, std::unique_ptr<Symbol> symbol)
{
  scopes_.back().insert(std::pair<std::string, std::unique_ptr<Symbol>>{name, std::move(symbol)});
}

std::optional<std::reference_wrapper<Symbol>> SymbolTable::lookup(const std::string& name, int maxDepth) const
{
  int depth = 1;
  for(auto scopesIt = scopes_.crbegin(); scopesIt < scopes_.crend(); ++scopesIt, ++depth)
  {
    auto it = scopesIt->find(name);
    if(it != scopesIt->end())
      return *it->second;

    if(maxDepth != 0 && depth == maxDepth)
      break;
  }

  return {};  
}
