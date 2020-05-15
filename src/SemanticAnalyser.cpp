#include "SemanticAnalyser.hpp"

void SemanticAnalyser::visit(const AssignmentNode& node) 
{
  const auto name = node.getName();
  const auto it = variables_.find(name);
  if(it == variables_.end())
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
  const auto it = functions_.find(name);
  if(it == functions_.end())
    throw std::runtime_error("ERROR: Calling undefined function named " + name + "!");

  const auto declaredArgs = it->second.first;
  const auto& callingArgs = node.getArguments();

  const auto nDeclaredArgs = declaredArgs.size();
  const auto nCallingArgs = callingArgs.size();

  if(nCallingArgs != nDeclaredArgs)
    throw std::runtime_error("ERROR: Function " + name + " expected " + 
      std::to_string(nDeclaredArgs) + " but got " + 
        std::to_string(nCallingArgs) + " arguments!");

  for(const auto& arg : callingArgs)
    arg->accept(*this);
}

void SemanticAnalyser::visit(const FunctionCallStatementNode& node) 
{
  node.getFunctionCall().accept(*this);
}

void SemanticAnalyser::visit(const FunctionDeclarationNode& node) 
{
  const auto name = node.getName();
  const auto it = functions_.find(name);
  if(it != functions_.end())
    throw std::runtime_error("ERROR: Redefinition of function named " + name + "!");

  
  const auto args = node.getArguments();
  const auto func = std::pair<ArgsList, ReturnType>(args, node.getReturnType());
  functions_.insert(std::pair<std::string, std::pair<ArgsList, ReturnType>>(name, func));

  node.getBody().accept(*this);
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
  node.getBody().accept(*this);
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
  const auto it = variables_.find(name);
  if(it != variables_.end())
    throw std::runtime_error("ERROR: Redefinition of variable " + name + "!");
  
  node.getValue().accept(*this);

  variables_.insert(std::pair<std::string, TypeName>{name, node.getType()});
}

void SemanticAnalyser::visit(const VariableNode& node) 
{
  const auto name = node.getName();
  const auto it = variables_.find(name);
  if(it == variables_.end())
    throw std::runtime_error("ERROR: Usage of undeclared variable " + name + "!");
}