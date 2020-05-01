#pragma once

class LiteralNode;
class VariableNode;
class UnaryNode;
class BinaryOpNode;

class Visitor
{
public:
  virtual void visit(const LiteralNode&) = 0;
  virtual void visit(const VariableNode&) = 0;
  virtual void visit(const UnaryNode&) = 0;
  virtual void visit(const BinaryOpNode&) = 0;
};