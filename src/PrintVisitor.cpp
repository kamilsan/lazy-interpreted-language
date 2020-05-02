#include "PrintVisitor.hpp"

#include "Node.hpp"

#include <iostream>

void PrintVisitor::visit(const LiteralNode& node)
{
  std::cout << indent() << "Literal: " << node.getValue() << "\n";
}

void PrintVisitor::visit(const VariableNode& node)
{
  std::cout << indent() << "Variable: " << node.getName() << "\n";
}

void PrintVisitor::visit(const UnaryNode& node)
{
  std::cout << indent() << "Unary: " 
    << UnaryOperationNames.at(node.getOperation()) << "\n";
  auto visitor = PrintVisitor{indentation_ + 1};
  node.getTerm().accept(visitor);
}

void PrintVisitor::visit(const BinaryOpNode& node)
{
  std::cout << indent() << "BinaryOpNode:\n";
  auto visitor = PrintVisitor{indentation_ + 1};
  node.getLeftOperand().accept(visitor);
  std::cout << indent() << BinaryOperationNames.at(node.getOperation()) << "\n";
  node.getRightOperand().accept(visitor);
}

void PrintVisitor::visit(const VariableDeclarationNode& node)
{
  std::cout << indent() << "VariableDeclarationNode:\n";
  auto visitor = PrintVisitor{indentation_ + 1};
  std::cout << indent() << node.getName() << "\n";
  node.getValue().accept(visitor);
}

void PrintVisitor::visit(const ReturnNode& node)
{
  std::cout << indent() << "ReturnNode:\n";
  auto visitor = PrintVisitor{indentation_ + 1};
  node.getValue().accept(visitor);
}