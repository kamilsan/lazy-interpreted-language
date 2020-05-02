#pragma once

class LiteralNode;
class VariableNode;
class UnaryNode;
class BinaryOpNode;
class VariableDeclarationNode;
class ReturnNode;

class Visitor
{
public:
  virtual void visit(const LiteralNode&) = 0;
  virtual void visit(const VariableNode&) = 0;
  virtual void visit(const UnaryNode&) = 0;
  virtual void visit(const BinaryOpNode&) = 0;
  virtual void visit(const VariableDeclarationNode&) = 0;
  virtual void visit(const ReturnNode&) = 0;
};