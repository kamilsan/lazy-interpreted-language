#include "Executor.hpp"

#include <cmath>
#include <iostream>

#include "AST.hpp"

void Executor::visit(const AssignmentNode& node)
{
  const auto name = node.getName();
  auto symbol = context_.lookup(name);
  if(node.getOperation() == AssignmentOperator::Assign)
  {
    ValueChanger valueChanger{node.getValue()};
    symbol.value().get().accept(valueChanger);
  }
  else
  {
    RuntimeVariableAnalyser analyser{};
    symbol.value().get().accept(analyser);
    Executor executor{analyser.getContext()};
    analyser.getValue()->accept(executor);

    NumberValueAnalyser valueAnalyser{};
    executor.getValue()->accept(valueAnalyser);
    const auto oldValue = valueAnalyser.getValue().value();

    node.getValue()->accept(*this);
    value_->accept(valueAnalyser);
    const auto rhs = valueAnalyser.getValue().value();

    double newValue = oldValue;
    switch(node.getOperation())
    {
      case AssignmentOperator::PlusEq:
        newValue = oldValue + rhs;
        break;
      case AssignmentOperator::MinusEq:
        newValue = oldValue - rhs;
        break;
      case AssignmentOperator::MulEq:
        newValue = oldValue * rhs;
        break;
      case AssignmentOperator::DivEq:
        newValue = oldValue / rhs;
        break;
      case AssignmentOperator::OrEq:
        newValue = static_cast<unsigned int>(oldValue) | static_cast<unsigned int>(rhs);
        break;
      case AssignmentOperator::AndEq:
        newValue = static_cast<unsigned int>(oldValue) & static_cast<unsigned int>(rhs);
        break;
      case AssignmentOperator::XorEq:
        newValue = static_cast<unsigned int>(oldValue) ^ static_cast<unsigned int>(rhs);
        break;
      case AssignmentOperator::ShiftLeftEq:
        newValue = static_cast<unsigned int>(oldValue) << static_cast<unsigned int>(rhs);
        break;
      case AssignmentOperator::ShiftRightEq:
        newValue = static_cast<unsigned int>(oldValue) >> static_cast<unsigned int>(rhs);
        break;
      case AssignmentOperator::Assign:
        break; // Unreachable
    }

    ValueChanger valueChanger{std::make_shared<NumericLiteralNode>(newValue)};
    symbol.value().get().accept(valueChanger);
  }
}

void Executor::visit(const BinaryOpNode& node)
{
  node.getLeftOperand().accept(*this);
  auto left = std::move(value_);

  node.getRightOperand().accept(*this);
  auto right = std::move(value_);

  if(node.getOperation() == BinaryOperator::Addition)
  {
    if(left->getType() == TypeName::String)
    {
      StringValueAnalyser valueAnalyser{};
      left->accept(valueAnalyser);

      const auto l = valueAnalyser.getValue().value();
      if(right->getType() == TypeName::String)
      {
        right->accept(valueAnalyser);
        const auto r = valueAnalyser.getValue().value();
        value_ = std::make_unique<String>(l + r);
      }
      else if(right->getType() == TypeName::F32)
      {
        NumberValueAnalyser numberValueAnalyser{};
        right->accept(numberValueAnalyser);

        const auto r = numberValueAnalyser.getValue().value();
        value_ = std::make_unique<String>(l + std::to_string(r));
      }
      else
        throw std::runtime_error(""); // TODO
    }
    else
    {
      NumberValueAnalyser numberValueAnalyser{};
      left->accept(numberValueAnalyser);
      const auto l = numberValueAnalyser.getValue().value();
      right->accept(numberValueAnalyser);
      const auto r = numberValueAnalyser.getValue().value();

      value_ = std::make_unique<Number>(l + r);
    }
  }
  else {
    NumberValueAnalyser valueAnalyser{};
    left->accept(valueAnalyser);
    const auto l = valueAnalyser.getValue().value();

    right->accept(valueAnalyser);
    const auto r = valueAnalyser.getValue().value();

    auto newValue = l;

    switch (node.getOperation())
    {
      case BinaryOperator::BinaryAnd:
        newValue = static_cast<unsigned int>(l) & static_cast<unsigned int>(r);
        break;
      case BinaryOperator::BinaryOr:
        newValue = static_cast<unsigned int>(l) | static_cast<unsigned int>(r);
        break;
      case BinaryOperator::BinaryXor:
        newValue = static_cast<unsigned int>(l) ^ static_cast<unsigned int>(r);
        break;
      case BinaryOperator::Division:
        newValue = l / r;
        break;
      case BinaryOperator::Equal:
        newValue = l == r ? 1 : 0;
        break;
        case BinaryOperator::Greater:
        newValue = l > r ? 1 : 0;
        break;
      case BinaryOperator::GreaterEq:
        newValue = l >= r ? 1 : 0;
        break;
      case BinaryOperator::Less:
        newValue = l < r ? 1 : 0;
        break;
      case BinaryOperator::LessEq:
        newValue = l <= r ? 1 : 0;
        break;
      case BinaryOperator::LogicalAnd:
        newValue = l && r;
        break;
      case BinaryOperator::LogicalOr:
        newValue = l || r;
        break;
      case BinaryOperator::Modulo:
        newValue = std::fmod(l, r);
        break;
      case BinaryOperator::Multiplication:
        newValue = l * r;
        break;
      case BinaryOperator::NotEqual:
        newValue = l != r ? 1 : 0;
        break;
      case BinaryOperator::ShiftLeft:
        newValue = static_cast<unsigned int>(l) << static_cast<unsigned int>(r);
        break;
      case BinaryOperator::ShiftRight:
        newValue = static_cast<unsigned int>(l) >> static_cast<unsigned int>(r);
        break;
      case BinaryOperator::Subtraction:
        newValue = l - r;
        break;
      case BinaryOperator::Addition:
        break; // Unreachable
    }

    value_ = std::make_unique<Number>(newValue);
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

    StringValueAnalyser analyser{};
    value_->accept(analyser);

    const auto str = analyser.getValue().value();
    std::cout << str << "\n";
  }
  else if(name == "if")
  {
    const auto& args = node.getArguments();
    auto it = args.begin();
    (*it)->accept(*this);

    NumberValueAnalyser analyser{};
    value_->accept(analyser);
    const auto condition = analyser.getValue().value();

    if(std::fabs(condition) > 0.0001)
    {
      it++;
      (*it)->accept(*this);
    }
    else
    {
      args.back()->accept(*this);
    }
  }
  else
  {
    const auto symbol = context_.lookup(name);
    auto functionAnalyser = RuntimeFunctionAnalyser{};
    symbol.value().get().accept(functionAnalyser);

    context_.enterScope();

    auto it = node.getArguments().begin();
    for(const auto& arg : functionAnalyser.getArguments())
    {
      const auto argName = arg.first;
      const auto type = arg.second;
      std::shared_ptr<ExpressionNode> value = *it;
      auto argSymbol = std::make_unique<RuntimeVariableSymbol>(argName, type, value, context_.clone());
      context_.addSymbol(argName, std::move(argSymbol));

      ++it;
    }

    functionAnalyser.getBody()->accept(*this);

    context_.leaveScope();

    if(functionAnalyser.getReturnType() != TypeName::Void)
    {
      value_ = std::move(returnStack_.top());
      returnStack_.pop();
    }
  }
}

void Executor::visit(const FunctionCallStatementNode& node)
{
  node.getFunctionCall().accept(*this);
}

void Executor::visit(const FunctionDeclarationNode& node)
{
  const auto name = node.getName();
  const auto type = node.getReturnType();
  auto symbol = std::make_unique<RuntimeFunctionSymbol>(name, type, node.getBody());
  for(const auto& arg : node.getArguments())
  {
    symbol->addArgument(RuntimeFunctionSymbol::Argument{arg.first, arg.second});
  }

  context_.addSymbol(name, std::move(symbol));
}

void Executor::visit(const FunctionResultCallNode&)
{

}

void Executor::visit(const LambdaCallNode& node)
{
  const auto& lambda = node.getLambda();
  context_.enterScope();

  auto it = node.getArguments().begin();
  for(const auto& arg : lambda.getArguments())
  {
    const auto argName = arg.first;
    const auto type = arg.second;
    std::shared_ptr<ExpressionNode> value = *it;
    auto argSymbol = std::make_unique<RuntimeVariableSymbol>(argName, type, value, context_.clone());
    context_.addSymbol(argName, std::move(argSymbol));

    ++it;
  }

  lambda.getBody().accept(*this);

  context_.leaveScope();

}

void Executor::visit(const LambdaNode&)
{

}

void Executor::visit(const NumericLiteralNode& node)
{
  value_ = std::make_unique<Number>(node.getValue());
}

void Executor::visit(const ProgramNode& node)
{
  for(const auto& variable : node.getVariables())
    variable->accept(*this);

  for(const auto& function : node.getFunctions())
    function->accept(*this);

  const auto mainSymbol = context_.lookup("main");
  auto functionAnalyser = RuntimeFunctionAnalyser{};
  mainSymbol.value().get().accept(functionAnalyser);

  context_.enterScope();
  functionAnalyser.getBody()->accept(*this);
  context_.leaveScope();
}

void Executor::visit(const ReturnNode& node)
{
  node.getValue().accept(*this);
  auto returnedValue = std::move(value_);
  returnStack_.emplace(std::move(returnedValue));
}

void Executor::visit(const StringLiteralNode& node)
{
  value_ = std::make_unique<String>(node.getValue());
}

void Executor::visit(const UnaryNode& node)
{
  node.getTerm().accept(*this);

  NumberValueAnalyser analyser{};
  value_->accept(analyser);
  auto term = analyser.getValue().value();
  auto newValue = term;
  switch(node.getOperation())
  {
    case UnaryOperator::BinaryNegation:
      newValue = ~static_cast<unsigned int>(term);
      break;
    case UnaryOperator::LogicalNot:
      newValue = term == 0 ? 1 : 0;
      break;
    case UnaryOperator::Minus:
      newValue = -term;
      break;
  }

  value_ = std::make_unique<Number>(newValue);
}

void Executor::visit(const VariableDeclarationNode& node)
{
  const auto name = node.getName();
  const auto type = node.getType();
  auto value = node.getValue();

  auto symbol = std::make_unique<RuntimeVariableSymbol>(name, type, value, context_.clone());
  context_.addSymbol(name, std::move(symbol));
}

void Executor::visit(const VariableNode& node)
{
  const auto name = node.getName();
  const auto symbol = context_.lookup(name);

  RuntimeVariableAnalyser analyser{};
  symbol.value().get().accept(analyser);

  const auto& value = analyser.getValue();
  auto executor = Executor{analyser.getContext()};

  value->accept(executor);
  value_ = executor.getValue()->clone();
}
