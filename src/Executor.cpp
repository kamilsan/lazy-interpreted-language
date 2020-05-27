#include "Executor.hpp"

#include <cmath>

#include "AST.hpp"

void Executor::visit(const AssignmentNode&)
{
  
}

void Executor::visit(const BinaryOpNode& node)
{
  auto leftExecutor = Executor{};
  auto rightExecutor = Executor{};

  node.getLeftOperand().accept(leftExecutor);
  node.getRightOperand().accept(rightExecutor);

  auto left = leftExecutor.getValue().value();
  auto right = rightExecutor.getValue().value();

  switch(node.getOperation())
  {
    case BinaryOperator::Addition:
      value_ = left + right;
      break;
    case BinaryOperator::BinaryAnd:
      value_ = static_cast<int>(left) & static_cast<int>(right);
      break;
    case BinaryOperator::BinaryOr:
      value_ =  static_cast<int>(left) | static_cast<int>(right);
      break;
    case BinaryOperator::BinaryXor:
      value_ =  static_cast<int>(left) ^ static_cast<int>(right);
      break;
    case BinaryOperator::Division:
      value_ = left / right;
      break;
    case BinaryOperator::Equal:
      value_ = left == right ? 1 : 0;
      break;
    case BinaryOperator::Greater:
      value_ = left > right ? 1 : 0;
      break;
    case BinaryOperator::GreaterEq:
      value_ = left >= right ? 1 : 0;
      break;
    case BinaryOperator::Less:
      value_ = left < right ? 1 : 0;
      break;
    case BinaryOperator::LessEq:
      value_ = left <= right ? 1 : 0;
      break;
    case BinaryOperator::LogicalAnd:
      value_ = left && right;
      break;
    case BinaryOperator::LogicalOr:
      value_ = left || right;
      break;
    case BinaryOperator::Modulo:
      value_ = std::fmod(left, right);
      break;
    case BinaryOperator::Multiplication:
      value_ = left * right;
      break;
    case BinaryOperator::NotEqual:
      value_ = left != right ? 1 : 0;
      break;
    case BinaryOperator::ShiftLeft:
      value_ = static_cast<int>(left) << static_cast<int>(right);
      break;
    case BinaryOperator::ShiftRight:
      value_ = static_cast<int>(left) >> static_cast<int>(right);
      break;
    case BinaryOperator::Subtraction:
      value_ = left - right;
      break;
  }
}

void Executor::visit(const BlockNode&)
{
  
}

void Executor::visit(const FunctionCallNode&)
{
  
}

void Executor::visit(const FunctionCallStatementNode&)
{
  
}

void Executor::visit(const FunctionDeclarationNode&)
{
  
}

void Executor::visit(const FunctionResultCallNode&)
{
  
}

void Executor::visit(const LambdaCallNode&)
{
  
}

void Executor::visit(const LambdaNode&)
{
  
}

void Executor::visit(const NumericLiteralNode& node)
{
  value_ = node.getValue();
}

void Executor::visit(const ProgramNode&)
{
  
}

void Executor::visit(const ReturnNode&)
{
  
}

void Executor::visit(const StringLiteralNode&)
{
  
}

void Executor::visit(const UnaryNode& node)
{
  auto termExecutor = Executor{};
  node.getTerm().accept(termExecutor);
  auto term = termExecutor.getValue().value();

  switch(node.getOperation())
  {
    case UnaryOperator::BinaryNegation:
      value_ = !term;
      break;
    case UnaryOperator::LogicalNot:
      value_ = term == 0 ? 1 : 0;
      break;
    case UnaryOperator::Minus:
      value_ = -term;
      break;
  }
}

void Executor::visit(const VariableDeclarationNode&)
{
  
}

void Executor::visit(const VariableNode&)
{
  
}
