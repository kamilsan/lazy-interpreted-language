#pragma once

class ProgramNode;
class NumericLiteralNode;
class StringLiteralNode;
class VariableNode;
class UnaryNode;
class BinaryOpNode;
class VariableDeclarationNode;
class AssignmentNode;
class ReturnNode;
class BlockNode;
class FunctionDeclarationNode;
class FunctionCallNode;
class FunctionResultCallNode;
class FunctionCallStatementNode;
class LambdaNode;
class LambdaCallNode;

class Visitor
{
public:
  virtual void visit(const ProgramNode&) = 0;
  virtual void visit(const NumericLiteralNode&) = 0;
  virtual void visit(const StringLiteralNode&) = 0;
  virtual void visit(const VariableNode&) = 0;
  virtual void visit(const UnaryNode&) = 0;
  virtual void visit(const BinaryOpNode&) = 0;
  virtual void visit(const VariableDeclarationNode&) = 0;
  virtual void visit(const AssignmentNode&) = 0;
  virtual void visit(const ReturnNode&) = 0;
  virtual void visit(const BlockNode&) = 0;
  virtual void visit(const FunctionDeclarationNode&) = 0;
  virtual void visit(const FunctionCallNode&) = 0;
  virtual void visit(const FunctionResultCallNode&) = 0;
  virtual void visit(const FunctionCallStatementNode&) = 0;
  virtual void visit(const LambdaNode&) = 0;
  virtual void visit(const LambdaCallNode&) = 0;
};