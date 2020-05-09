#pragma once

#include "Visitor.hpp"

#include <string>

class PrintVisitor : public Visitor
{
public:
  PrintVisitor(unsigned int indentation = 0): indentation_(indentation) {}

  void visit(const ProgramNode&) override;
  void visit(const NumericLiteralNode&) override;
  void visit(const StringLiteralNode&) override;
  void visit(const VariableNode&) override;
  void visit(const UnaryNode&) override;
  void visit(const BinaryOpNode&) override;
  void visit(const VariableDeclarationNode&) override;
  void visit(const AssignmentNode&) override;
  void visit(const ReturnNode&) override;
  void visit(const BlockNode&) override;
  void visit(const FunctionDeclarationNode&) override;
  void visit(const FunctionCallNode&) override;
  void visit(const FunctionResultCallNode&) override;
  void visit(const FunctionCallStatementNode&) override;
  void visit(const LambdaNode&) override;
  void visit(const LambdaCallNode&) override;

private:
  std::string indent() { return std::string(indentation_, ' '); }

  unsigned int indentation_;
};