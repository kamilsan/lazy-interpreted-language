#pragma once

#include "Visitor.hpp"

#include <string>

class PrintVisitor : public Visitor
{
public:
  PrintVisitor(unsigned int indentation = 0): indentation_(indentation) {}

  void visit(const LiteralNode&) override;
  void visit(const TermNode&) override;
  void visit(const UnaryNode&) override;
  void visit(const BinaryOpNode&) override;
private:
  std::string indent() { return std::string(indentation_, ' '); }

  unsigned int indentation_;
};