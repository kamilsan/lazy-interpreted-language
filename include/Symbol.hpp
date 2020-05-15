#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <deque>
#include <list>
#include <optional>
#include <functional>

#include "AST.hpp"

class Symbol
{
public:
  virtual ~Symbol() = 0;
};

class VariableSymbol : public Symbol
{
public:
  VariableSymbol(const std::string& name, const TypeName& type):
    name_(name), type_(type) {}

  const std::string& getName() const { return name_; }
  const TypeName& getType() const { return type_; }

private:
  std::string name_;
  TypeName type_;
};

class FunctionSymbol : public Symbol
{
public:
  FunctionSymbol(const std::string& name, const TypeName& returnType, 
    const std::list<std::pair<std::string, TypeName>>& arguments):
      name_(name), returnType_(returnType), arguments_(arguments) {}
  FunctionSymbol(const std::string& name, const TypeName& returnType):
    name_(name), returnType_(returnType), arguments_() {}

  const std::string& getName() const { return name_; }
  const TypeName& getReturnType() const { return returnType_; }
  const std::list<std::pair<std::string, TypeName>>& getArguments() const { return arguments_; }

  void addArgument(const std::string& name, const TypeName& type)
  {
    arguments_.push_back(std::pair<std::string, TypeName>(name, type));
  }

private:
  std::string name_;
  TypeName returnType_;
  std::list<std::pair<std::string, TypeName>> arguments_;
};

class SymbolTable
{
public:
  SymbolTable();

  void enterScope();
  void leaveScope();
  void addSymbol(const std::string& name, std::unique_ptr<Symbol> symbol);
  std::optional<std::reference_wrapper<Symbol>> lookup(const std::string& name) const;

private:
  std::deque<std::unordered_map<std::string, std::unique_ptr<Symbol>>> scopes_;
};