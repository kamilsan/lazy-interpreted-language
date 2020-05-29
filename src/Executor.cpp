#include "Executor.hpp"

#include <cmath>
#include <iostream>

#include "AST.hpp"

void Executor::visit(const AssignmentNode&)
{
  
}

void Executor::visit(const BinaryOpNode& node)
{
  node.getLeftOperand().accept(*this);
  auto left = value_;

  node.getRightOperand().accept(*this);
  auto right = value_;

  switch(node.getOperation())
  {
    case BinaryOperator::Addition:
    {
      if(std::holds_alternative<std::string>(left))
      {
        const auto l = std::get<std::string>(left);
        if(std::holds_alternative<std::string>(right))
        {
          const auto r = std::get<std::string>(right);
          value_ = l + r;
        }
        else
        {
          const auto r = std::get<double>(right);
          value_ = l + std::to_string(r);
        }
      }
      else
        value_ = std::get<double>(left) + std::get<double>(right);
      break;
    }
    case BinaryOperator::BinaryAnd:
    {
      const auto l = std::get<double>(left);
      const auto r = std::get<double>(right);
      value_ = static_cast<int>(l) & static_cast<int>(r);
      break;
    }
    case BinaryOperator::BinaryOr:
    {
      const auto l = std::get<double>(left);
      const auto r = std::get<double>(right);
      value_ =  static_cast<int>(l) | static_cast<int>(r);
      break;
    }
    case BinaryOperator::BinaryXor:
    {
      const auto l = std::get<double>(left);
      const auto r = std::get<double>(right);
      value_ =  static_cast<int>(l) ^ static_cast<int>(r);
      break;
    }
    case BinaryOperator::Division:
    {
      const auto l = std::get<double>(left);
      const auto r = std::get<double>(right);
      value_ = l / r;
      break;
    }
    case BinaryOperator::Equal:
    {
      const auto l = std::get<double>(left);
      const auto r = std::get<double>(right);
      value_ = l == r ? 1 : 0;
      break;
    }
    case BinaryOperator::Greater:
    {
      const auto l = std::get<double>(left);
      const auto r = std::get<double>(right);
      value_ = l > r ? 1 : 0;
      break;
    }
    case BinaryOperator::GreaterEq:
    {
      const auto l = std::get<double>(left);
      const auto r = std::get<double>(right);
      value_ = l >= r ? 1 : 0;
      break;
    }
    case BinaryOperator::Less:
    {
      const auto l = std::get<double>(left);
      const auto r = std::get<double>(right);
      value_ = l < r ? 1 : 0;
      break;
    }
    case BinaryOperator::LessEq:
    {
      const auto l = std::get<double>(left);
      const auto r = std::get<double>(right);
      value_ = l <= r ? 1 : 0;
      break;
    }
    case BinaryOperator::LogicalAnd:
    {
      const auto l = std::get<double>(left);
      const auto r = std::get<double>(right);
      value_ = l && r;
      break;
    }
    case BinaryOperator::LogicalOr:
    {
      const auto l = std::get<double>(left);
      const auto r = std::get<double>(right);
      value_ = l || r;
      break;
    }
    case BinaryOperator::Modulo:
    {
      const auto l = std::get<double>(left);
      const auto r = std::get<double>(right);
      value_ = std::fmod(l, r);
      break;
    }
    case BinaryOperator::Multiplication:
    {
      const auto l = std::get<double>(left);
      const auto r = std::get<double>(right);
      value_ = l * r;
      break;
    }
    case BinaryOperator::NotEqual:
    {
      const auto l = std::get<double>(left);
      const auto r = std::get<double>(right);
      value_ = l != r ? 1 : 0;
      break;
    }
    case BinaryOperator::ShiftLeft:
    {
      const auto l = std::get<double>(left);
      const auto r = std::get<double>(right);
      value_ = static_cast<int>(l) << static_cast<int>(r);
      break;
    }
    case BinaryOperator::ShiftRight:
    {
      const auto l = std::get<double>(left);
      const auto r = std::get<double>(right);
      value_ = static_cast<int>(l) >> static_cast<int>(r);
      break;
    }
    case BinaryOperator::Subtraction:
    {
      const auto l = std::get<double>(left);
      const auto r = std::get<double>(right);
      value_ = l - r;
      break;
    }
  }
}

void Executor::visit(const BlockNode& node)
{
  for(const auto& statement: node.getStatements())
   statement->accept(*this);
}

void Executor::visit(const FunctionCallNode& node)
{
  const auto name = node.getName();
  if(name == "print")
  {
    const auto& args = node.getArguments();
    (*args.begin())->accept(*this);
    const auto str = std::get<std::string>(value_);
    std::cout << str << "\n";
  }
}

void Executor::visit(const FunctionCallStatementNode& node)
{
  node.getFunctionCall().accept(*this);
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

void Executor::visit(const StringLiteralNode& node)
{
  value_ = node.getValue();
}

void Executor::visit(const UnaryNode& node)
{
  auto termExecutor = Executor{};
  node.getTerm().accept(termExecutor);
  auto term = std::get<double>(termExecutor.getValue());

  switch(node.getOperation())
  {
    case UnaryOperator::BinaryNegation:
      value_ = ~static_cast<int>(term);
      break;
    case UnaryOperator::LogicalNot:
      value_ = term == 0 ? 1 : 0;
      break;
    case UnaryOperator::Minus:
      value_ = -term;
      break;
  }
}

void Executor::visit(const VariableDeclarationNode& node)
{
  const auto name = node.getName();
  const auto type = node.getType();
  auto value = node.getValue();

  auto symbol = std::make_unique<RuntimeVariableSymbol>(name, type, value);
  context_.addSymbol(name, std::move(symbol));
}

void Executor::visit(const VariableNode& node)
{
  const auto name = node.getName();
  const auto symbol = context_.lookup(name);

  RuntimeVariableAnalyser analyser{};
  symbol.value().get().accept(analyser);

  const auto& value = analyser.getValue();
  value->accept(*this);
}
