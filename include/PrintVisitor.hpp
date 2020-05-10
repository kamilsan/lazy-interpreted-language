#pragma once

#include "Visitor.hpp"

#include <iostream>
#include <string>

class PrintVisitor : public Visitor
{
public:
  PrintVisitor(std::ostream& stream = std::cout, unsigned int indentation = 0): 
    stream_(stream), indentation_(indentation) {}

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
  std::string indent() { return std::string(indentation_, ' '); }

  std::ostream& stream_;
  unsigned int indentation_;
};
