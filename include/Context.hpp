#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <deque>
#include <list>
#include <optional>
#include <functional>

#include "AST.hpp"

class RuntimeSymbol;
class RuntimeVariableSymbol;
class RuntimeFunctionSymbol;

class RuntimeSymbolVisitor
{
public:
  virtual void visit(RuntimeVariableSymbol&) = 0;
  virtual void visit(RuntimeFunctionSymbol&) = 0;
};

class Context
{
public:
  Context();

  void debug() const;
  Context clone() const;
  void enterScope();
  void leaveScope();
  void addSymbol(const std::string& name, std::shared_ptr<RuntimeSymbol> symbol);
  std::optional<std::reference_wrapper<RuntimeSymbol>> lookup(const std::string& name, int maxDepth = 0) const;

private:
  std::deque<std::unordered_map<std::string, std::shared_ptr<RuntimeSymbol>>> scopes_;
};

class RuntimeVariableAnalyser: public RuntimeSymbolVisitor
{
public:
  RuntimeVariableAnalyser();

  bool isSymbolValid() const { return symbolValid_; }
  std::optional<TypeName> getType() const { return type_; }
  const std::shared_ptr<ExpressionNode>& getValue() const { return value_; }
  const Context& getContext() const { return context_->get(); }

  void visit(RuntimeVariableSymbol&) override;
  void visit(RuntimeFunctionSymbol&) override;
private:
  bool symbolValid_;
  std::optional<TypeName> type_;
  std::shared_ptr<ExpressionNode> value_;
  std::optional<std::reference_wrapper<const Context>> context_;
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
  virtual std::shared_ptr<RuntimeSymbol> clone(const Context& context) const = 0;
};

class RuntimeVariableSymbol : public RuntimeSymbol
{
public:
  RuntimeVariableSymbol(const std::string& name, const TypeName& type,
          std::shared_ptr<ExpressionNode> value, const Context& context):
    name_(name), type_(type), value_(std::move(value)), context_(context)
  {}

  const std::string& getName() const { return name_; }
  const TypeName& getType() const { return type_; }
  const std::shared_ptr<ExpressionNode>& getValue() const { return value_; }
  const Context& getContext() const { return context_; }

  void setValue(std::shared_ptr<ExpressionNode> value) { value_ = std::move(value); }

  std::shared_ptr<RuntimeSymbol> clone(const Context& context) const override;
  void accept(RuntimeSymbolVisitor& visitor) override { visitor.visit(*this); };
private:
  std::string name_;
  TypeName type_;
  std::shared_ptr<ExpressionNode> value_;
  Context context_;
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

  std::shared_ptr<RuntimeSymbol> clone(const Context& context) const override;
  void accept(RuntimeSymbolVisitor& visitor) override { visitor.visit(*this); };
private:
  std::string name_;
  TypeName returnType_;
  ArgumentsList arguments_;
  std::shared_ptr<BlockNode> body_;
};
