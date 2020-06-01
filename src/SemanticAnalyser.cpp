#include "SemanticAnalyser.hpp"

#include "Common.hpp"
#include "TypeChecker.hpp"

SemanticAnalyser::SemanticAnalyser(): symbols_(), hasReturn_()
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
    reportError("Assignment to undeclared variable " + name, node);

  VariableAnalyser analyser{};
  symbol.value().get().accept(analyser);
  if(!analyser.isSymbolValid())
    reportError("Assignment to a non-variable symbol " + name, node);

  if(analyser.getType() == TypeName::Function && node.getOperation() != AssignmentOperator::Assign)
    reportError("Cannot perform arithmetic operation on function variable " + name, node);

  node.getValue()->accept(*this);

  TypeChecker typeChecker{symbols_};
  node.getValue()->accept(typeChecker);

  // Type checker is not be able to deduce expression's type when variable was called
  // and in that case getType has no value.
  if(typeChecker.getType().has_value() && typeChecker.getType() != analyser.getType())
    reportError("Cannot assign value of type " + 
      TypeNameStrings.at(typeChecker.getType().value()) + " to variable " + name + "!", node);
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
    reportError("Calling undefined function named " + name + "!", node);

  FunctionAnalyser analyser{};
  symbol.value().get().accept(analyser);
  if(!analyser.isSymbolValid())
    reportError("Symbol " + name + " does not name a function!", node);

  /*
   * If user calls variable of type function, then analyser knows nothing about it's return type and arguments.
   * This is because variables' values are not being tracked.
   */
  if(analyser.getReturnType().has_value())
  {
    const auto expectedArgs = analyser.getArguments();
    const auto& providedArgs = node.getArguments();
    const auto nExpectedArgs = expectedArgs.size();
    const auto nProvidedArgs = providedArgs.size();

    if(nExpectedArgs != nProvidedArgs)
      reportError("Function " + name + " expected " +
        std::to_string(nExpectedArgs) + ", but got " + std::to_string(nProvidedArgs) + " arguments!", node);

    auto expectedArgsIt = expectedArgs.begin();
    for(const auto& arg : node.getArguments())
    {
      arg->accept(*this);

      TypeChecker typeChecker{symbols_};
      arg->accept(typeChecker);
      if(typeChecker.getType().has_value() && typeChecker.getType() != *expectedArgsIt)
        reportError("Function " + name + " expected argument of type " + 
          TypeNameStrings.at(*expectedArgsIt) + ", but got " + 
            TypeNameStrings.at(typeChecker.getType().value()) + "!", node);
      
      expectedArgsIt++;
    }
  }
}

void SemanticAnalyser::visit(const FunctionCallStatementNode& node) 
{
  node.getFunctionCall().accept(*this);
}

void SemanticAnalyser::visit(const FunctionDeclarationNode& node) 
{
  const auto name = node.getName();
  const auto symbol = symbols_.lookup(name, 1);
  if(symbol)
    reportError("Redefinition of function named " + name + "!", node);
 
  const auto args = node.getArguments();
  auto functionSymbol = std::make_unique<FunctionSymbol>(name, node.getReturnType());
  for(const auto& arg : args)
  {
    functionSymbol->addArgument(arg.second);
  }

  symbols_.addSymbol(name, std::move(functionSymbol));

  hasReturn_.push({});
  symbols_.enterScope();

  for(const auto& arg: args)
  {
    symbols_.addSymbol(arg.first, std::make_unique<VariableSymbol>(arg.first, arg.second));
  }

  node.getBody()->accept(*this);
  
  symbols_.leaveScope();
  const auto returnInfo = hasReturn_.top();
  hasReturn_.pop();

  if(node.getReturnType() != TypeName::Void && !returnInfo.hasReturn)
    reportError("Function " + name + " does not return any value!", node);
  else if(node.getReturnType() == TypeName::Void && returnInfo.hasReturn)
    reportError("Void function " + name + " does return!", node);
  else if(node.getReturnType() != TypeName::Void && 
    returnInfo.type.has_value() && node.getReturnType() != returnInfo.type.value())
    reportError("Function " + name + " should return " + 
      TypeNameStrings.at(node.getReturnType()) + ", but returns " + 
        TypeNameStrings.at(returnInfo.type.value()) + "!", node);
}

void SemanticAnalyser::visit(const FunctionResultCallNode& node) 
{
  node.getCall().accept(*this);

  TypeChecker typeChecker{symbols_};
  node.getCall().accept(typeChecker);
  if(typeChecker.getType().has_value() && typeChecker.getType() != TypeName::Function)
    reportError("Cannot call result of function returning " + 
      TypeNameStrings.at(typeChecker.getType().value()) + "!", node);
  
  for(const auto& arg : node.getArguments())
    arg->accept(*this);
}

void SemanticAnalyser::visit(const LambdaCallNode& node) 
{
  const auto& lambda = node.getLambda();
  const auto& expectedArguments = lambda.getArguments();
  const auto& providedArguments = node.getArguments();
  const auto nExpectedArgs = expectedArguments.size();
  const auto nProvidedArgs = providedArguments.size();

  lambda.accept(*this);

  if(nExpectedArgs != nProvidedArgs)
    reportError("Lambda expected " + std::to_string(nExpectedArgs) + 
      ", but got " + std::to_string(nProvidedArgs) + " arguments!", node);


  auto expectedArgumentIt = expectedArguments.begin();
  for(const auto& arg : providedArguments)
  {
    arg->accept(*this);

    TypeChecker typeChecker{symbols_};
    arg->accept(typeChecker);
    if(typeChecker.getType().has_value() && typeChecker.getType() != expectedArgumentIt->second)
      reportError("Lambda expected argument of type " + 
        TypeNameStrings.at(expectedArgumentIt->second) + ", but got " + 
          TypeNameStrings.at(typeChecker.getType().value()) + "!", node);

    expectedArgumentIt++;
  }
}

void SemanticAnalyser::visit(const LambdaNode& node) 
{
  hasReturn_.push({});
  symbols_.enterScope();

  for(const auto& arg : node.getArguments())
  {
    symbols_.addSymbol(arg.first, std::make_unique<VariableSymbol>(arg.first, arg.second));
  }

  node.getBody().accept(*this);

  symbols_.leaveScope();
  const auto returnInfo = hasReturn_.top();
  hasReturn_.pop();

  if(node.getReturnType() != TypeName::Void && !returnInfo.hasReturn)
    reportError("Lambda does not return any value!", node);
  else if(node.getReturnType() == TypeName::Void && returnInfo.hasReturn)
    reportError("Void lambda does return!", node);
  else if(node.getReturnType() != TypeName::Void && 
    returnInfo.type.has_value() && node.getReturnType() != returnInfo.type.value())
    reportError("Lambda should return " + 
      TypeNameStrings.at(node.getReturnType()) + ", but returns " + 
        TypeNameStrings.at(returnInfo.type.value()) + "!", node);
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
    reportError("Main function was not found!", node);
  
  FunctionAnalyser analyser{};
  symbol.value().get().accept(analyser);
  if(!analyser.isSymbolValid())
    reportError("Symbol main does not name a function!", node);
  else if(analyser.getReturnType() != TypeName::F32)
    reportError("Main should return F32!", node);
}

void SemanticAnalyser::visit(const ReturnNode& node) 
{
  node.getValue().accept(*this);
  TypeChecker typeChecker{symbols_};
  node.getValue().accept(typeChecker);

  if(typeChecker.getType().has_value())
    hasReturn_.top() = ReturnInfo{typeChecker.getType().value()};
  else
    hasReturn_.top() = ReturnInfo{true};
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
  const auto symbol = symbols_.lookup(name, 1);
  if(symbol)
    reportError("Redefinition of variable " + name + "!", node);
  
  node.getValue()->accept(*this);

  TypeChecker typeChecker{symbols_};
  node.getValue()->accept(typeChecker);

  // Type checker is not be able to deduce expression's type when variable was called
  // and in that case getType has no value.
  if(typeChecker.getType().has_value() && typeChecker.getType() != node.getType())
    reportError("Cannot assign value of type " + 
      TypeNameStrings.at(typeChecker.getType().value()) + " to variable " + name + "!", node);

  auto variableSymbol = std::make_unique<VariableSymbol>(name, node.getType());
  symbols_.addSymbol(name, std::move(variableSymbol));
}

void SemanticAnalyser::visit(const VariableNode& node) 
{
  const auto name = node.getName();
  const auto symbol = symbols_.lookup(name);
  if(!symbol)
    reportError("Usage of undeclared symbol " + name + "!", node);
}