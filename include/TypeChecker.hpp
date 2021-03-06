#pragma once

#include "Visitor.hpp"
#include "AST.hpp"
#include "Symbol.hpp"

class TypeChecker : public Visitor
{
public:
  TypeChecker(const SymbolTable& symbols): type_(), symbols_(symbols) {}

  const std::optional<TypeName>& getType() const;

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
  std::optional<TypeName> type_;
  const SymbolTable& symbols_;
};