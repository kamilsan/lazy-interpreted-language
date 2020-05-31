#pragma once

#include "Visitor.hpp"
#include "Context.hpp"

#include <variant>
#include <string>
#include <stack>

#include <iostream>

class Executor : public Visitor
{
public:
  Executor(): value_(), context_(), returnStack_() {}
  Executor(const Context& context): value_(), context_(context), returnStack_() {}

  const std::variant<double, std::string>& getValue() const { return value_; }

  void visit(const AssignmentNode&) override;
  void visit(const BinaryOpNode&) override;
  void visit(const BlockNode&) override;
  void visit(const FunctionCallNode&) override;
  void visit(const FunctionCallStatementNode&) override;
  void visit(const FunctionDeclarationNode&) override;
  void visit(const FunctionResultCallNode&) override;
  void visit(const LambdaCallNode&) override;
  void visit(const LambdaNode&) override;
  void visit(const NumericLiteralNode&) override;
  void visit(const ProgramNode&) override;
  void visit(const ReturnNode&) override;
  void visit(const StringLiteralNode&) override;
  void visit(const UnaryNode&) override;
  void visit(const VariableDeclarationNode&) override;
  void visit(const VariableNode&) override;

private:
  using Value = std::variant<double, std::string>;

  Value value_;
  Context context_;
  std::stack<Value> returnStack_;
};
