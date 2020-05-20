#include "TypeChecker.hpp"


const std::optional<TypeName>& TypeChecker::getType() const
{
  return type_;
}

void TypeChecker::visit(const AssignmentNode&)
{}

void TypeChecker::visit(const BinaryOpNode& node)
{
  TypeChecker checker{symbols_};
  node.getLeftOperand().accept(checker);  
  auto leftType = checker.getType().value();

  node.getRightOperand().accept(checker);  
  auto rightType = checker.getType().value();

  if(leftType == TypeName::String)
  {
    if(node.getOperation() != BinaryOperator::Addition)
      throw std::runtime_error("ERROR: Invalid operation on value of type " + 
        TypeNameStrings.at(leftType) + "!");

    type_ = TypeName::String;
  }
  else if(leftType == TypeName::F32 && rightType == TypeName::F32)
    type_ = TypeName::F32;
  else
    throw std::runtime_error("ERROR: Invalid operation on value of type " + 
        TypeNameStrings.at(leftType) + "!");
}

void TypeChecker::visit(const BlockNode&)
{}

void TypeChecker::visit(const FunctionCallNode& node)
{
  auto name = node.getName();
  auto& symbol = symbols_.lookup(name).value().get();
  FunctionAnalyserVisitor analyser{};
  symbol.accept(analyser);
  type_ = analyser.getReturnType();
}

void TypeChecker::visit(const FunctionCallStatementNode&)
{}

void TypeChecker::visit(const FunctionDeclarationNode&)
{}

void TypeChecker::visit(const FunctionResultCallNode& node)
{
  TypeChecker checker{symbols_};
  node.getCall().accept(checker);
  type_ = checker.getType();
}

void TypeChecker::visit(const LambdaCallNode& node)
{
  type_ = node.getLambda().getReturnType();
}

void TypeChecker::visit(const LambdaNode&)
{
  type_ = TypeName::Function;
}

void TypeChecker::visit(const NumericLiteralNode&)
{
  type_ = TypeName::F32;
}

void TypeChecker::visit(const ProgramNode&)
{}

void TypeChecker::visit(const ReturnNode&)
{}

void TypeChecker::visit(const StringLiteralNode&)
{
  type_ = TypeName::String;
}

void TypeChecker::visit(const UnaryNode& node)
{
  TypeChecker checker{symbols_};
  node.getTerm().accept(checker);
  auto termType = checker.getType().value();
  if(termType != TypeName::F32)
    throw std::runtime_error("ERROR: Invalid operation on value of type " + 
      TypeNameStrings.at(termType) + "!");
  
  type_ = termType;
}

void TypeChecker::visit(const VariableDeclarationNode&)
{}

void TypeChecker::visit(const VariableNode& node)
{
  auto name = node.getName();
  auto& symbol = symbols_.lookup(name).value().get();
  VariableAnalyserVisitor analyser{};
  symbol.accept(analyser);
  if(analyser.isSymbolValid())
    type_ = analyser.getType().value();
  else
  {
    FunctionAnalyserVisitor analyser{};
    symbol.accept(analyser);
    if(analyser.isSymbolValid())
      type_ = TypeName::Function;
    else
      throw std::runtime_error("ERROR: Invalid symbol reference!");
  }
}