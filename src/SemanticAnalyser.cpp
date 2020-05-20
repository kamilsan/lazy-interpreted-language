#include "SemanticAnalyser.hpp"

#include "TypeChecker.hpp"

SemanticAnalyser::SemanticAnalyser(): symbols_()
{
  addBuildInSymbols();
}

void SemanticAnalyser::addBuildInSymbols()
{
  auto ifSymbol = std::make_unique<FunctionSymbol>("if", TypeName::F32);
  ifSymbol->addArgument(TypeName::F32);
  ifSymbol->addArgument(TypeName::F32);
  ifSymbol->addArgument(TypeName::F32);

  auto printSymbol = std::make_unique<FunctionSymbol>("print", TypeName::Void);
  printSymbol->addArgument(TypeName::String);

  symbols_.addSymbol("if", std::move(ifSymbol));
  symbols_.addSymbol("print", std::move(printSymbol));
}

void SemanticAnalyser::visit(const AssignmentNode& node) 
{
  const auto name = node.getName();
  const auto symbol = symbols_.lookup(name);
  if(!symbol)
    throw std::runtime_error("ERROR: Assignment to undeclared variable " + name);

  VariableAnalyserVisitor analyser{};
  symbol.value().get().accept(analyser);
  if(!analyser.isSymbolValid())
    throw std::runtime_error("ERROR: Assignment to a non-variable symbol " + name);

  node.getValue().accept(*this);

  TypeChecker typeChecker{symbols_};
  node.getValue().accept(typeChecker);
  if(typeChecker.getType() != analyser.getType())
    throw std::runtime_error("ERROR: Cannot assign value of type " + 
      TypeNameStrings.at(typeChecker.getType().value()) + " to variable " + name + "!");
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

  FunctionAnalyserVisitor analyser{};
  symbol.value().get().accept(analyser);
  if(!analyser.isSymbolValid())
    throw std::runtime_error("ERROR: Symbol " + name + " does not name a function!");

  const auto expectedArgs = analyser.getArguments();
  const auto& providedArgs = node.getArguments();
  const auto nExpectedArgs = expectedArgs.size();
  const auto nProvidedArgs = providedArgs.size();

  if(nExpectedArgs != nProvidedArgs)
    throw std::runtime_error("ERROR: Function " + name + " expected " +
      std::to_string(nExpectedArgs) + ", but got " + std::to_string(nProvidedArgs) + " arguments!");

  auto expectedArgsIt = expectedArgs.begin();
  for(const auto& arg : node.getArguments())
  {
    arg->accept(*this);

    TypeChecker typeChecker{symbols_};
    arg->accept(typeChecker);
    if(typeChecker.getType() != *expectedArgsIt)
      throw std::runtime_error("ERROR: Function expected other type!");
    
    expectedArgsIt++;
  }
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
  auto functionSymbol = std::make_unique<FunctionSymbol>(name, node.getReturnType());
  for(const auto& arg : args)
  {
    functionSymbol->addArgument(arg.second);
  }

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

  const auto symbol = symbols_.lookup("main");
  if(!symbol)
    throw std::runtime_error("ERROR: Main function was not found!");
  
  FunctionAnalyserVisitor analyser{};
  symbol.value().get().accept(analyser);
  if(!analyser.isSymbolValid())
    throw std::runtime_error("ERROR: Symbol main does not name a function!");
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

  TypeChecker typeChecker{symbols_};
  node.getValue().accept(typeChecker);
  if(typeChecker.getType() != node.getType())
    throw std::runtime_error("ERROR: Cannot assign value of type " + 
      TypeNameStrings.at(typeChecker.getType().value()) + " to variable " + name + "!");

  auto variableSymbol = std::make_unique<VariableSymbol>(name, node.getType());
  symbols_.addSymbol(name, std::move(variableSymbol));
}

void SemanticAnalyser::visit(const VariableNode& node) 
{
  const auto name = node.getName();
  const auto symbol = symbols_.lookup(name);
  if(!symbol)
    throw std::runtime_error("ERROR: Usage of undeclared variable " + name + "!");

  VariableAnalyserVisitor analyser{};
  symbol.value().get().accept(analyser);
  if(!analyser.isSymbolValid())
    throw std::runtime_error("ERROR: Reference to nonvariable symbol " + name);
}