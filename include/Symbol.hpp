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
  virtual ~SymbolVisitor() = default;

  virtual void visit(VariableSymbol&) = 0;
  virtual void visit(FunctionSymbol&) = 0;
};

class VariableAnalyser: public SymbolVisitor
{
public:
  VariableAnalyser();

  bool isSymbolValid() const { return symbolValid_; }
  std::optional<TypeName> getType() const { return type_; }

  void visit(VariableSymbol&) override;
  void visit(FunctionSymbol&) override;
private:
  bool symbolValid_;
  std::optional<TypeName> type_;
};

class FunctionAnalyser: public SymbolVisitor
{
public:
  FunctionAnalyser();

  bool isSymbolValid() const { return symbolValid_; }
  std::optional<TypeName> getReturnType() const { return returnType_; }
  const std::list<TypeName>& getArguments() const { return arguments_; }

  void visit(VariableSymbol&) override;
  void visit(FunctionSymbol&) override;
private:
  bool symbolValid_;
  std::optional<TypeName> returnType_;
  std::list<TypeName> arguments_;
};

class Symbol
{
public:
  virtual ~Symbol() = default;
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
  std::optional<std::reference_wrapper<Symbol>> lookup(const std::string& name, int maxDepth = 0) const;

private:
  std::deque<std::unordered_map<std::string, std::unique_ptr<Symbol>>> scopes_;
};