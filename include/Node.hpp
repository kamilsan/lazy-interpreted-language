#pragma once

#include <memory>
#include <string>
#include <sstream>
#include <unordered_map>
#include <list>

#include "Token.hpp"
#include "Visitor.hpp"

enum class UnaryOperation
{
  BinaryNegation,
  Minus,
  LogicalNot
};

enum class BinaryOperation
{
  Addition,
  Subtraction,
  Multiplication,
  Division,
  Modulo,
  LogicalAnd,
  LogicalOr,
  BinaryAnd,
  BinaryOr,
  BinaryXor,
  ShiftLeft,
  ShiftRight,
  Greater,
  GreaterEq,
  Less,
  LessEq,
  Equal,
  NotEqual
};

enum class TypeName
{
  F32,
  Function
};

const std::unordered_map<UnaryOperation, std::string> UnaryOperationNames = {
  std::make_pair<UnaryOperation, std::string>(UnaryOperation::BinaryNegation, "BinaryNegation"),
  std::make_pair<UnaryOperation, std::string>(UnaryOperation::Minus, "Minus"),
  std::make_pair<UnaryOperation, std::string>(UnaryOperation::LogicalNot, "LogicalNot")
};

const std::unordered_map<BinaryOperation, std::string> BinaryOperationNames = {
  std::make_pair<BinaryOperation, std::string>(BinaryOperation::Addition, "Addition"),
  std::make_pair<BinaryOperation, std::string>(BinaryOperation::Subtraction, "Subtraction"),
  std::make_pair<BinaryOperation, std::string>(BinaryOperation::Multiplication, "Multiplication"),
  std::make_pair<BinaryOperation, std::string>(BinaryOperation::Division, "Division"),
  std::make_pair<BinaryOperation, std::string>(BinaryOperation::Modulo, "Modulo"),
  std::make_pair<BinaryOperation, std::string>(BinaryOperation::LogicalAnd, "LogicalAnd"),
  std::make_pair<BinaryOperation, std::string>(BinaryOperation::LogicalOr, "LogicalOr"),
  std::make_pair<BinaryOperation, std::string>(BinaryOperation::BinaryAnd, "BinaryAnd"),
  std::make_pair<BinaryOperation, std::string>(BinaryOperation::BinaryOr, "BinaryOr"),
  std::make_pair<BinaryOperation, std::string>(BinaryOperation::BinaryXor, "BinaryXor"),
  std::make_pair<BinaryOperation, std::string>(BinaryOperation::ShiftLeft, "ShiftLeft"),
  std::make_pair<BinaryOperation, std::string>(BinaryOperation::ShiftRight, "ShiftRight"),
  std::make_pair<BinaryOperation, std::string>(BinaryOperation::Greater, "Greater"),
  std::make_pair<BinaryOperation, std::string>(BinaryOperation::GreaterEq, "GreaterEq"),
  std::make_pair<BinaryOperation, std::string>(BinaryOperation::Less, "Less"),
  std::make_pair<BinaryOperation, std::string>(BinaryOperation::LessEq, "LessEq"),
  std::make_pair<BinaryOperation, std::string>(BinaryOperation::Equal, "Equal"),
  std::make_pair<BinaryOperation, std::string>(BinaryOperation::NotEqual, "NotEqual")
};

const std::unordered_map<TypeName, std::string> TypeNameStrings = {
  std::make_pair<TypeName, std::string>(TypeName::F32, "f32"),
  std::make_pair<TypeName, std::string>(TypeName::Function, "function")
};

class Node
{
public:
  virtual void accept(Visitor&) const = 0;
};

class ExpressionNode : public Node
{
public:
  virtual void accept(Visitor&) const = 0;
};

class StatementNode : public Node
{
public:
  virtual void accept(Visitor&) const = 0;
};

class ProgramNode : public Node
{
public:
  ProgramNode(): variables_(), functions_() {}

  void addVariable(std::unique_ptr<VariableDeclarationNode> variable) 
    { variables_.push_back(std::move(variable)); }
  void addFunction(std::unique_ptr<FunctionDeclarationNode> function) 
    { functions_.push_back(std::move(function)); }

  const std::list<std::unique_ptr<VariableDeclarationNode>>& getVariables() 
    const { return variables_; }
  const std::list<std::unique_ptr<FunctionDeclarationNode>>& getFunctions() 
    const { return functions_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  std::list<std::unique_ptr<VariableDeclarationNode>> variables_;
  std::list<std::unique_ptr<FunctionDeclarationNode>> functions_;
};

class LiteralNode : public ExpressionNode
{
public:
  LiteralNode(double value): value_(value) {}

  double getValue() const { return value_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  double value_;
};

class VariableNode : public ExpressionNode
{
public:
  VariableNode(const std::string& name): name_(name) {}

  std::string getName() const { return name_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  std::string name_;
};

class UnaryNode : public ExpressionNode
{
public:
  UnaryNode(const UnaryOperation& unaryOp, std::unique_ptr<ExpressionNode> term):
    unaryOperation_(unaryOp), term_(std::move(term)) {}

  const ExpressionNode& getTerm() const { return *term_; }
  const UnaryOperation& getOperation() const { return unaryOperation_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  UnaryOperation unaryOperation_;
  std::unique_ptr<ExpressionNode> term_;
};

class BinaryOpNode : public ExpressionNode
{
public:
  BinaryOpNode(std::unique_ptr<ExpressionNode> leftOperand, const BinaryOperation& op, std::unique_ptr<ExpressionNode> rightOperand):
    leftOperand_(std::move(leftOperand)), operation_(op), rightOperand_(std::move(rightOperand)) {}

  const ExpressionNode& getLeftOperand() const { return *leftOperand_; }
  const ExpressionNode& getRightOperand() const { return *rightOperand_; }
  const BinaryOperation& getOperation() const { return operation_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  std::unique_ptr<ExpressionNode> leftOperand_;
  BinaryOperation operation_;
  std::unique_ptr<ExpressionNode> rightOperand_;
};

class FunctionCallNode : public ExpressionNode
{
public:
  FunctionCallNode(const std::string& name, std::list<std::unique_ptr<ExpressionNode>> arguments):
    name_(name), arguments_(std::move(arguments)) {}

  const std::string& getName() const { return name_; }
  const std::list<std::unique_ptr<ExpressionNode>>& getArguments() const { return arguments_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  std::string name_;
  std::list<std::unique_ptr<ExpressionNode>> arguments_;
};

class VariableDeclarationNode : public StatementNode
{
public:
  VariableDeclarationNode(const std::string& name, const TypeName& type, std::unique_ptr<ExpressionNode> value):
    name_(name), type_(type), value_(std::move(value)) {}

  const std::string& getName() const { return name_; }
  const TypeName& getType() const { return type_; }
  const ExpressionNode& getValue() const { return *value_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  std::string name_;
  TypeName type_;
  std::unique_ptr<ExpressionNode> value_;
};

class ReturnNode : public StatementNode
{
public:
  ReturnNode(std::unique_ptr<ExpressionNode> value):
    value_(std::move(value)) {}

  const ExpressionNode& getValue() const { return *value_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  std::unique_ptr<ExpressionNode> value_;
};

class BlockNode : public StatementNode
{
public:
  BlockNode(): statements_{} {}

  void addStatement(std::unique_ptr<StatementNode> statement) { statements_.push_back(std::move(statement)); }
  const std::list<std::unique_ptr<StatementNode>>& getStatements() const { return statements_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  std::list<std::unique_ptr<StatementNode>> statements_;  
};

class FunctionDeclarationNode : public StatementNode
{
public:
  FunctionDeclarationNode(const std::string& name, const TypeName& returnType, 
    const std::list<std::pair<std::string, TypeName>> args, std::unique_ptr<BlockNode> body):
      name_(name), returnType_(returnType), arguments_(args), body_(std::move(body)) {}

  const std::string& getName() const { return name_; }
  const TypeName& getReturnType() const { return returnType_; }
  const std::list<std::pair<std::string, TypeName>>& getArguments() const { return arguments_; }
  const BlockNode& getBody() const { return *body_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  std::string name_;
  TypeName returnType_;
  std::list<std::pair<std::string, TypeName>> arguments_;
  std::unique_ptr<BlockNode> body_;
};

class FunctionCallStatementNode : public StatementNode
{
public:
  FunctionCallStatementNode(std::unique_ptr<ExpressionNode> functionCallNode):
    functionCall_(std::move(functionCallNode)) {}

  const ExpressionNode& getFunctionCall() const { return *functionCall_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  std::unique_ptr<ExpressionNode> functionCall_;
};
