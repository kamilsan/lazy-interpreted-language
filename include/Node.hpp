#pragma once

#include <memory>
#include <string>
#include <sstream>
#include <unordered_map>

#include "Token.hpp"
#include "Visitor.hpp"

enum class UnaryOperation
{
  BinaryNegation,
  Minus
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

const std::unordered_map<UnaryOperation, std::string> UnaryOperationNames = {
  std::make_pair<UnaryOperation, std::string>(UnaryOperation::BinaryNegation, "BinaryNegation"),
  std::make_pair<UnaryOperation, std::string>(UnaryOperation::Minus, "Minus")
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

class LiteralNode : public ExpressionNode
{
public:
  LiteralNode(double value): value_(value) {}

  double getValue() const { return value_; }
  void setValue(double value) { value_ = value; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  double value_;
};

class VariableNode : public ExpressionNode
{
public:
  VariableNode(const std::string& name): name_(name) {}

  std::string getName() const { return name_; }
  void setValue(double name) { name_ = name; }

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
  BinaryOpNode(std::unique_ptr<Node> leftOperand, const BinaryOperation& op, std::unique_ptr<Node> rightOperand):
    leftOperand_(std::move(leftOperand)), operation_(op), rightOperand_(std::move(rightOperand)) {}

  const Node& getLeftOperand() const { return *leftOperand_; }
  const Node& getRightOperand() const { return *rightOperand_; }
  const BinaryOperation& getOperation() const { return operation_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  std::unique_ptr<Node> leftOperand_;
  BinaryOperation operation_;
  std::unique_ptr<Node> rightOperand_;
};

