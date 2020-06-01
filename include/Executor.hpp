#pragma once

#include "Visitor.hpp"
#include "Context.hpp"
#include "Value.h"

#include <string>
#include <stack>

class Executor : public Visitor
{
public:
  Executor(): value_(), context_(), returnStack_() {}
  Executor(const Context& context): value_(), context_(context), returnStack_() {}

  Executor(const Executor&) = delete;

  const std::unique_ptr<Value>& getValue() const { return value_; }

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
  void handlePrint(const FunctionCallNode&);
  void handleIf(const FunctionCallNode&);
  void handleVariableCall(const FunctionCallNode&, const RuntimeVariableAnalyser&);
  void handleFunctionCall(const FunctionCallNode&, const RuntimeFunctionAnalyser&);

  void assertValueType(const Value& value, const TypeName& type, const std::string& activity, const Node& node) const;
  [[noreturn]] void reportError(const std::string& message, const Node& node) const;

  std::unique_ptr<Value> value_;
  Context context_;
  std::stack<std::unique_ptr<Value>> returnStack_;
};
