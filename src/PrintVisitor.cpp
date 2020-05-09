#include "PrintVisitor.hpp"

#include "Node.hpp"

#include <iostream>

void PrintVisitor::visit(const ProgramNode& node)
{
  std::cout << indent() << "ProgramNode:\n";
  auto visitor = PrintVisitor{indentation_ + 1};

  std::cout << "\nVariables:\n";
  for(const auto& variable : node.getVariables())
  {
    variable->accept(visitor);
  }

  std::cout << "\nFunctions:\n";
  for(const auto& function : node.getFunctions())
  {
    function->accept(visitor);
  }
}

void PrintVisitor::visit(const NumericLiteralNode& node)
{
  std::cout << indent() << "NumericLiteral: " << node.getValue() << "\n";
}

void PrintVisitor::visit(const StringLiteralNode& node)
{
  std::cout << indent() << "StringLiteral: " << node.getValue() << "\n";
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
  std::cout << indent() << "VariableDeclarationNode (" 
    << TypeNameStrings.at(node.getType()) << "):\n";
  std::cout << indent() << "Name: " << node.getName() << "\n";
  auto visitor = PrintVisitor{indentation_ + 1};
  std::cout << indent() << "Value:\n";
  node.getValue().accept(visitor);
}

void PrintVisitor::visit(const AssignmentNode& node)
{
  std::cout << indent() << "AssignmentNode:\n";
  std::cout << indent() << "Name: " << node.getName() << "\n";
  std::cout << indent() << "Operator: " << 
    AssignmentOperationNames.at(node.getOperation()) << "\n";
    
  auto visitor = PrintVisitor{indentation_ + 1};
  std::cout << indent() << "Value:\n";
  node.getValue().accept(visitor);
}

void PrintVisitor::visit(const ReturnNode& node)
{
  std::cout << indent() << "ReturnNode:\n";
  auto visitor = PrintVisitor{indentation_ + 1};
  node.getValue().accept(visitor);
}

void PrintVisitor::visit(const BlockNode& node)
{
  std::cout << indent() << "BlockNode:\n";
  auto visitor = PrintVisitor{indentation_ + 1};
  for(const auto& statement : node.getStatements())
  {
    statement->accept(visitor);
  }
}

void PrintVisitor::visit(const FunctionDeclarationNode& node)
{
  std::cout << indent() << "FunctionDeclarationNode (" 
    << TypeNameStrings.at(node.getReturnType()) << "):\n";
  std::cout << indent() << "Name: " << node.getName() << "\n";
  std::cout << indent() << "Arguments:\n";
  for(const auto& arg : node.getArguments())
  {
    std::cout << indent() << " " << arg.first 
      << " (" << TypeNameStrings.at(arg.second) << ")\n";
  }
  auto visitor = PrintVisitor{indentation_ + 1};
  std::cout << indent() << "Body:\n";
  node.getBody().accept(visitor);
}

void PrintVisitor::visit(const FunctionCallNode& node)
{
  std::cout << indent() << "FunctionCallNode:\n";
  std::cout << indent() << "Name: " << node.getName() << "\n";
  std::cout << indent() << "Arguments:\n";
  auto visitor = PrintVisitor{indentation_ + 1};
  for(const auto& arg : node.getArguments())
  {
    arg->accept(visitor);
  }
}

void PrintVisitor::visit(const FunctionCallStatementNode& node)
{
  std::cout << indent() << "FunctionCallStatementNode:\n";
  auto visitor = PrintVisitor{indentation_ + 1};
  node.getFunctionCall().accept(visitor);
}

void PrintVisitor::visit(const LambdaNode& node)
{
  std::cout << indent() << "LambdaNode (" 
    << TypeNameStrings.at(node.getReturnType()) << "):\n";
  std::cout << indent() << "Arguments:\n";
  for(const auto& arg : node.getArguments())
  {
    std::cout << indent() << " " << arg.first 
      << " (" << TypeNameStrings.at(arg.second) << ")\n";
  }
  auto visitor = PrintVisitor{indentation_ + 1};
  std::cout << indent() << "Body:\n";
  node.getBody().accept(visitor);
}

void PrintVisitor::visit(const LambdaCallNode& node)
{
  std::cout << indent() << "LambdaCallNode:\n";
  std::cout << indent() << "Arguments:\n";
  auto visitor = PrintVisitor{indentation_ + 1};
  for(const auto& arg : node.getArguments())
  {
    arg->accept(visitor);
  }
  std::cout << indent() << "Lambda:\n";
  node.getLambda().accept(visitor);
}