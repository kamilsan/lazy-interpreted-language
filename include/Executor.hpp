#pragma once

#include "Visitor.hpp"
#include "Context.hpp"
#include "Value.h"

#include <string>
#include <stack>
#include <sstream>

class Executor : public Visitor
{
public:
  Executor(): value_(), context_(), returnStack_(), stdout_(), exitCode_(0) {}
  Executor(const Context& context): value_(), context_(context), returnStack_(), stdout_(), exitCode_(0) {}

  Executor(const Executor&) = delete;

  const std::unique_ptr<Value>& getValue() const { return value_; }
  int getExitCode() const { return exitCode_; }
  std::string getStandardOut() const { return stdout_.str(); }

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
  void callValue(const CallNode& node, const std::string& name, const Value& value);

  void assertValueType(const Value& value, const TypeName& type, const std::string& activity, const Node& node) const;

  std::unique_ptr<Value> value_;
  Context context_;
  std::stack<std::unique_ptr<Value>> returnStack_;
  std::ostringstream stdout_;
  int exitCode_;
};
