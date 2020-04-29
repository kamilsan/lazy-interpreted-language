#pragma once

#include <memory>
#include <string>
#include <sstream>

#include "Token.hpp"
#include "Visitor.hpp"

class Node
{
public:
  virtual void accept(Visitor&) const = 0;
};

class LiteralNode : public Node
{
public:
  LiteralNode(double value): value_(value) {}

  double getValue() const { return value_; }
  void setValue(double value) { value_ = value; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  double value_;
};

class TermNode : public Node
{
public:
  TermNode() {}

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
};

class UnaryNode : public Node
{
public:
  UnaryNode(const Token& unaryOp, std::unique_ptr<TermNode> term);

  const LiteralNode& getTerm() const { return *term_; }
  const Token& getOperation() const { return unaryOperation_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  Token unaryOperation_;
  std::unique_ptr<LiteralNode> term_;
};

class BinaryOpNode : public Node
{
public:
  BinaryOpNode(std::unique_ptr<Node> leftOperand, const Token& op, std::unique_ptr<Node> rightOperand):
    leftOperand_(std::move(leftOperand)), operation_(op), rightOperand_(std::move(rightOperand)) {}

  const Node& getLeftOperand() const { return *leftOperand_; }
  const Node& getRightOperand() const { return *rightOperand_; }
  const Token& getOperation() const { return operation_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  std::unique_ptr<Node> leftOperand_;
  Token operation_;
  std::unique_ptr<Node> rightOperand_;
};

