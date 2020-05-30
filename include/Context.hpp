#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <deque>
#include <list>
#include <optional>
#include <functional>

#include "AST.hpp"

class RuntimeVariableSymbol;
class RuntimeFunctionSymbol;

class RuntimeSymbolVisitor
{
public:
  virtual void visit(RuntimeVariableSymbol&) = 0;
  virtual void visit(RuntimeFunctionSymbol&) = 0;
};

class RuntimeVariableAnalyser: public RuntimeSymbolVisitor
{
public:
  RuntimeVariableAnalyser();

  bool isSymbolValid() const { return symbolValid_; }
  std::optional<TypeName> getType() const { return type_; }
  const std::shared_ptr<ExpressionNode>& getValue() const { return value_; }

  void visit(RuntimeVariableSymbol&) override;
  void visit(RuntimeFunctionSymbol&) override;
private:
  bool symbolValid_;
  std::optional<TypeName> type_;
  std::shared_ptr<ExpressionNode> value_;
};

class RuntimeFunctionAnalyser: public RuntimeSymbolVisitor
{
public:
  using ArgumentsList = std::list<std::pair<std::string, TypeName>>;

  RuntimeFunctionAnalyser();

  bool isSymbolValid() const { return symbolValid_; }
  std::optional<TypeName> getReturnType() const { return returnType_; }
  const ArgumentsList& getArguments() const { return arguments_; }
  const std::shared_ptr<BlockNode>& getBody() const { return body_; }

  void visit(RuntimeVariableSymbol&) override;
  void visit(RuntimeFunctionSymbol&) override;
private:
  bool symbolValid_;
  std::optional<TypeName> returnType_;
  ArgumentsList arguments_;
  std::shared_ptr<BlockNode> body_;
};

class ValueChanger : public RuntimeSymbolVisitor
{
public:
  ValueChanger(std::shared_ptr<ExpressionNode> value): value_(std::move(value)) {}

  void visit(RuntimeVariableSymbol&) override;
  void visit(RuntimeFunctionSymbol&) override;
private:
  std::shared_ptr<ExpressionNode> value_;
};

class RuntimeSymbol
{
public:
  virtual void accept(RuntimeSymbolVisitor&) = 0;
};

class RuntimeVariableSymbol : public RuntimeSymbol
{
public:
  RuntimeVariableSymbol(const std::string& name, const TypeName& type, std::shared_ptr<ExpressionNode> value):
    name_(name), type_(type), value_(std::move(value)) {}

  const std::string& getName() const { return name_; }
  const TypeName& getType() const { return type_; }
  const std::shared_ptr<ExpressionNode>& getValue() const { return value_; }

  void setValue(std::shared_ptr<ExpressionNode> value) { value_ = std::move(value); }

  void accept(RuntimeSymbolVisitor& visitor) { visitor.visit(*this); };
private:
  std::string name_;
  TypeName type_;
  std::shared_ptr<ExpressionNode> value_;
};

class RuntimeFunctionSymbol : public RuntimeSymbol
{
public:
  using Argument = std::pair<std::string, TypeName>;
  using ArgumentsList = std::list<Argument>;

  RuntimeFunctionSymbol(const std::string& name, const TypeName& returnType, 
    const ArgumentsList& arguments, std::shared_ptr<BlockNode> body):
      name_(name), returnType_(returnType), arguments_(arguments), body_(std::move(body)) {}
  RuntimeFunctionSymbol(const std::string& name, const TypeName& returnType, std::shared_ptr<BlockNode> body):
    name_(name), returnType_(returnType), arguments_(), body_(std::move(body)) {}

  const std::string& getName() const { return name_; }
  const TypeName& getReturnType() const { return returnType_; }
  const ArgumentsList& getArguments() const { return arguments_; }
  const std::shared_ptr<BlockNode>& getBody() const { return body_; }

  void addArgument(const Argument& type)
  {
    arguments_.push_back(type);
  }

  void accept(RuntimeSymbolVisitor& visitor) { visitor.visit(*this); };
private:
  std::string name_;
  TypeName returnType_;
  ArgumentsList arguments_;
  std::shared_ptr<BlockNode> body_;
};

class Context
{
public:
  Context();

  void enterScope();
  void leaveScope();
  void addSymbol(const std::string& name, std::unique_ptr<RuntimeSymbol> symbol);
  std::optional<std::reference_wrapper<RuntimeSymbol>> lookup(const std::string& name, int maxDepth = 0) const;

private:
  std::deque<std::unordered_map<std::string, std::unique_ptr<RuntimeSymbol>>> scopes_;
};