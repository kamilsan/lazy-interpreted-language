#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <deque>
#include <list>
#include <optional>
#include <functional>

#include "AST.hpp"

class VariableSymbol;
class FunctionSymbol;

class SymbolVisitor
{
public:
  virtual void visit(VariableSymbol&) = 0;
  virtual void visit(FunctionSymbol&) = 0;
};

class VariableAnalyserVisitor: public SymbolVisitor
{
public:
  VariableAnalyserVisitor();

  bool isSymbolValid() const { return symbolValid_; }
  std::optional<TypeName> getType() const { return type_; }

  void visit(VariableSymbol&) override;
  void visit(FunctionSymbol&) override;
private:
  bool symbolValid_;
  std::optional<TypeName> type_;
};

class FunctionAnalyserVisitor: public SymbolVisitor
{
public:
  FunctionAnalyserVisitor(const FunctionCallNode& node);

  bool isSymbolValid() const { return symbolValid_; }
  std::optional<TypeName> getType() const { return type_; }
  const std::optional<std::string>& getErrorMessage() const { return errorMsg_; }

  void visit(VariableSymbol&) override;
  void visit(FunctionSymbol&) override;
private:
  const FunctionCallNode& node_;
  bool symbolValid_;
  TypeName type_;
  std::optional<std::string> errorMsg_;
};

class Symbol
{
public:
  virtual void accept(SymbolVisitor&) = 0;
};

class VariableSymbol : public Symbol
{
public:
  VariableSymbol(const std::string& name, const TypeName& type):
    name_(name), type_(type) {}

  const std::string& getName() const { return name_; }
  const TypeName& getType() const { return type_; }

  void accept(SymbolVisitor& visitor) { visitor.visit(*this); };
private:
  std::string name_;
  TypeName type_;
};

class FunctionSymbol : public Symbol
{
public:
  FunctionSymbol(const std::string& name, const TypeName& returnType, 
    const std::list<TypeName>& arguments):
      name_(name), returnType_(returnType), arguments_(arguments) {}
  FunctionSymbol(const std::string& name, const TypeName& returnType):
    name_(name), returnType_(returnType), arguments_() {}

  const std::string& getName() const { return name_; }
  const TypeName& getReturnType() const { return returnType_; }
  const std::list<TypeName>& getArguments() const { return arguments_; }

  void addArgument(const TypeName& type)
  {
    arguments_.push_back(type);
  }

  void accept(SymbolVisitor& visitor) { visitor.visit(*this); };
private:
  std::string name_;
  TypeName returnType_;
  std::list<TypeName> arguments_;
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