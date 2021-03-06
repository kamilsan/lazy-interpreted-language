#pragma once

#include "AST.hpp"
#include "Visitor.hpp"
#include "Symbol.hpp"

#include <optional>
#include <stack>

class SemanticAnalyser : public Visitor
{
public:
  SemanticAnalyser();


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
  struct ReturnInfo
  {
    ReturnInfo(): hasReturn(false), type() {}
    ReturnInfo(bool returns): hasReturn(returns), type() {}
    ReturnInfo(TypeName type): hasReturn(true), type(type) {}

    bool hasReturn;
    std::optional<TypeName> type;
  };

  void addBuildInSymbols();

  SymbolTable symbols_;
  std::stack<ReturnInfo> hasReturn_;
};