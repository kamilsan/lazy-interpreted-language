#include "PrintVisitor.hpp"

#include "Node.hpp"

void PrintVisitor::visit(const ProgramNode& node)
{
  stream_ << indent() << "ProgramNode:\n";
  auto visitor = PrintVisitor{stream_, indentation_ + 1};

  stream_ << "\nVariables:\n";
  for(const auto& variable : node.getVariables())
  {
    variable->accept(visitor);
  }

  stream_ << "\nFunctions:\n";
  for(const auto& function : node.getFunctions())
  {
    function->accept(visitor);
  }
}

void PrintVisitor::visit(const NumericLiteralNode& node)
{
  stream_ << indent() << "NumericLiteral: " << node.getValue() << "\n";
}

void PrintVisitor::visit(const StringLiteralNode& node)
{
  stream_ << indent() << "StringLiteral: " << node.getValue() << "\n";
}

void PrintVisitor::visit(const VariableNode& node)
{
  stream_ << indent() << "Variable: " << node.getName() << "\n";
}

void PrintVisitor::visit(const UnaryNode& node)
{
  stream_ << indent() << "Unary: " 
    << UnaryOperationNames.at(node.getOperation()) << "\n";
  auto visitor = PrintVisitor{stream_, indentation_ + 1};
  node.getTerm().accept(visitor);
}

void PrintVisitor::visit(const BinaryOpNode& node)
{
  stream_ << indent() << "BinaryOpNode:\n";
  auto visitor = PrintVisitor{stream_, indentation_ + 1};
  node.getLeftOperand().accept(visitor);
  stream_ << indent() << BinaryOperationNames.at(node.getOperation()) << "\n";
  node.getRightOperand().accept(visitor);
}

void PrintVisitor::visit(const VariableDeclarationNode& node)
{
  stream_ << indent() << "VariableDeclarationNode (" 
    << TypeNameStrings.at(node.getType()) << "):\n";
  stream_ << indent() << "Name: " << node.getName() << "\n";
  auto visitor = PrintVisitor{stream_, indentation_ + 1};
  stream_ << indent() << "Value:\n";
  node.getValue().accept(visitor);
}

void PrintVisitor::visit(const AssignmentNode& node)
{
  stream_ << indent() << "AssignmentNode:\n";
  stream_ << indent() << "Name: " << node.getName() << "\n";
  stream_ << indent() << "Operator: " << 
    AssignmentOperationNames.at(node.getOperation()) << "\n";

  auto visitor = PrintVisitor{stream_, indentation_ + 1};
  stream_ << indent() << "Value:\n";
  node.getValue().accept(visitor);
}

void PrintVisitor::visit(const ReturnNode& node)
{
  stream_ << indent() << "ReturnNode:\n";
  auto visitor = PrintVisitor{stream_, indentation_ + 1};
  node.getValue().accept(visitor);
}

void PrintVisitor::visit(const BlockNode& node)
{
  stream_ << indent() << "BlockNode:\n";
  auto visitor = PrintVisitor{stream_, indentation_ + 1};
  for(const auto& statement : node.getStatements())
  {
    statement->accept(visitor);
  }
}

void PrintVisitor::visit(const FunctionDeclarationNode& node)
{
  stream_ << indent() << "FunctionDeclarationNode (" 
    << TypeNameStrings.at(node.getReturnType()) << "):\n";
  stream_ << indent() << "Name: " << node.getName() << "\n";
  stream_ << indent() << "Arguments:\n";
  for(const auto& arg : node.getArguments())
  {
    stream_ << indent() << " " << arg.first 
      << " (" << TypeNameStrings.at(arg.second) << ")\n";
  }
  auto visitor = PrintVisitor{stream_, indentation_ + 1};
  stream_ << indent() << "Body:\n";
  node.getBody().accept(visitor);
}

void PrintVisitor::visit(const FunctionCallNode& node)
{
  stream_ << indent() << "FunctionCallNode:\n";
  stream_ << indent() << "Name: " << node.getName() << "\n";
  stream_ << indent() << "Arguments:\n";
  auto visitor = PrintVisitor{stream_, indentation_ + 1};
  for(const auto& arg : node.getArguments())
  {
    arg->accept(visitor);
  }
}

void PrintVisitor::visit(const FunctionResultCallNode& node)
{
  stream_ << indent() << "FunctionResultCallNode:\n";

  auto visitor = PrintVisitor{stream_, indentation_ + 1};
  stream_ << indent() << "Function:\n";
  node.getCall().accept(visitor);
  
  stream_ << indent() << "Arguments:\n";
  for(const auto& arg : node.getArguments())
  {
    arg->accept(visitor);
  }
}

void PrintVisitor::visit(const FunctionCallStatementNode& node)
{
  stream_ << indent() << "FunctionCallStatementNode:\n";
  auto visitor = PrintVisitor{stream_, indentation_ + 1};
  node.getFunctionCall().accept(visitor);
}

void PrintVisitor::visit(const LambdaNode& node)
{
  stream_ << indent() << "LambdaNode (" 
    << TypeNameStrings.at(node.getReturnType()) << "):\n";
  stream_ << indent() << "Arguments:\n";
  for(const auto& arg : node.getArguments())
  {
    stream_ << indent() << " " << arg.first 
      << " (" << TypeNameStrings.at(arg.second) << ")\n";
  }
  auto visitor = PrintVisitor{stream_, indentation_ + 1};
  stream_ << indent() << "Body:\n";
  node.getBody().accept(visitor);
}

void PrintVisitor::visit(const LambdaCallNode& node)
{
  stream_ << indent() << "LambdaCallNode:\n";
  stream_ << indent() << "Arguments:\n";
  auto visitor = PrintVisitor{stream_, indentation_ + 1};
  for(const auto& arg : node.getArguments())
  {
    arg->accept(visitor);
  }
  stream_ << indent() << "Lambda:\n";
  node.getLambda().accept(visitor);
}