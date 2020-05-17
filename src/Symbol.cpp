#include "Symbol.hpp"


VariableAnalyserVisitor::VariableAnalyserVisitor():
  symbolValid_(false), type_() {}

void VariableAnalyserVisitor::visit(VariableSymbol& symbol)
{
  type_ = symbol.getType();
  symbolValid_ = true;
}

void VariableAnalyserVisitor::visit(FunctionSymbol&)
{
  symbolValid_ = false;
}

FunctionAnalyserVisitor::FunctionAnalyserVisitor(const FunctionCallNode& node):
  node_(node), symbolValid_(false), type_(), errorMsg_() {}

void FunctionAnalyserVisitor::visit(VariableSymbol&)
{
  symbolValid_ = false;
}

void FunctionAnalyserVisitor::visit(FunctionSymbol& symbol)
{
  const auto nExpectedArgs = symbol.getArguments().size();
  const auto nProvidedArgs = node_.getArguments().size(); 
  if(nExpectedArgs != nProvidedArgs)
  {
    errorMsg_ = "Function named " + node_.getName() + " expected " + 
      std::to_string(nExpectedArgs) + ", but got " + 
        std::to_string(nProvidedArgs) + " arguments!";
  }
  else
    symbolValid_ = true;

  type_ = symbol.getReturnType();
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

std::optional<std::reference_wrapper<Symbol>> SymbolTable::lookup(const std::string& name) const
{
  for(auto scopesIt = scopes_.crbegin(); scopesIt < scopes_.crend(); ++scopesIt)
  {
    auto it = scopesIt->find(name);
    if(it != scopesIt->end())
      return *it->second;
  }

  return {};  
}
