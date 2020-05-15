#include "SemanticAnalyser.hpp"

void SemanticAnalyser::visit(const AssignmentNode& node) 
{
  const auto name = node.getName();
  const auto symbol = symbols_.lookup(name);
  if(!symbol)
    throw std::runtime_error("ERROR: Assignment to undeclared variable " + name);

  node.getValue().accept(*this);
}

void SemanticAnalyser::visit(const BinaryOpNode& node) 
{
  node.getLeftOperand().accept(*this);
  node.getRightOperand().accept(*this);
}

void SemanticAnalyser::visit(const BlockNode& node) 
{
  for(const auto& statement : node.getStatements())
    statement->accept(*this);
}

void SemanticAnalyser::visit(const FunctionCallNode& node) 
{
  const auto name = node.getName();
  const auto symbol = symbols_.lookup(name);
  if(!symbol)
    throw std::runtime_error("ERROR: Calling undefined function named " + name + "!");

  for(const auto& arg : node.getArguments())
    arg->accept(*this);
}

void SemanticAnalyser::visit(const FunctionCallStatementNode& node) 
{
  node.getFunctionCall().accept(*this);
}

void SemanticAnalyser::visit(const FunctionDeclarationNode& node) 
{
  const auto name = node.getName();
  const auto symbol = symbols_.lookup(name);
  if(symbol)
    throw std::runtime_error("ERROR: Redefinition of function named " + name + "!");

  
  const auto args = node.getArguments();
  auto functionSymbol = std::make_unique<FunctionSymbol>(name, node.getReturnType(), args);
  symbols_.addSymbol(name, std::move(functionSymbol));

  symbols_.enterScope();
  for(const auto& arg: args)
  {
    symbols_.addSymbol(arg.first, std::make_unique<VariableSymbol>(arg.first, arg.second));
  }
  node.getBody().accept(*this);
  symbols_.leaveScope();
}

void SemanticAnalyser::visit(const FunctionResultCallNode& node) 
{
  for(const auto& arg : node.getArguments())
    arg->accept(*this);
}

void SemanticAnalyser::visit(const LambdaCallNode& node) 
{
  node.getLambda().accept(*this);
  for(const auto& arg : node.getArguments())
    arg->accept(*this);
}

void SemanticAnalyser::visit(const LambdaNode& node) 
{
  symbols_.enterScope();
  for(const auto& arg: node.getArguments())
  {
    symbols_.addSymbol(arg.first, std::make_unique<VariableSymbol>(arg.first, arg.second));
  }
  node.getBody().accept(*this);
  symbols_.leaveScope();
}

void SemanticAnalyser::visit(const NumericLiteralNode&) 
{}

void SemanticAnalyser::visit(const ProgramNode& node) 
{
  for(const auto& var : node.getVariables())
    var->accept(*this);
  
  for(const auto& func : node.getFunctions())
    func->accept(*this);
}

void SemanticAnalyser::visit(const ReturnNode& node) 
{
  node.getValue().accept(*this);
}

void SemanticAnalyser::visit(const StringLiteralNode&) 
{}

void SemanticAnalyser::visit(const UnaryNode& node) 
{
  node.getTerm().accept(*this);
}


void SemanticAnalyser::visit(const VariableDeclarationNode& node) 
{
  const auto name = node.getName();
  const auto symbol = symbols_.lookup(name);
  if(symbol)
    throw std::runtime_error("ERROR: Redefinition of variable " + name + "!");
  
  node.getValue().accept(*this);

  auto variableSymbol = std::make_unique<VariableSymbol>(name, node.getType());
  symbols_.addSymbol(name, std::move(variableSymbol));
}

void SemanticAnalyser::visit(const VariableNode& node) 
{
  const auto name = node.getName();
  const auto symbol = symbols_.lookup(name);
  if(!symbol)
    throw std::runtime_error("ERROR: Usage of undeclared variable " + name + "!");
}