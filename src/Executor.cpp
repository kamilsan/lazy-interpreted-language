#include "Executor.hpp"

#include <cmath>
#include <iostream>

#include "AST.hpp"

void Executor::visit(const AssignmentNode& node)
{
  const auto name = node.getName();
  auto symbol = context_.lookup(name);
  ValueChanger valueChanger{node.getValue()};
  symbol.value().get().accept(valueChanger);
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
      value_ = static_cast<unsigned int>(l) & static_cast<unsigned int>(r);
      break;
    }
    case BinaryOperator::BinaryOr:
    {
      const auto l = std::get<double>(left);
      const auto r = std::get<double>(right);
      value_ =  static_cast<unsigned int>(l) | static_cast<unsigned int>(r);
      break;
    }
    case BinaryOperator::BinaryXor:
    {
      const auto l = std::get<double>(left);
      const auto r = std::get<double>(right);
      value_ =  static_cast<unsigned int>(l) ^ static_cast<unsigned int>(r);
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
      value_ = static_cast<unsigned int>(l) << static_cast<unsigned int>(r);
      break;
    }
    case BinaryOperator::ShiftRight:
    {
      const auto l = std::get<double>(left);
      const auto r = std::get<double>(right);
      value_ = static_cast<unsigned int>(l) >> static_cast<unsigned int>(r);
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
  else if(name == "if")
  {
    const auto& args = node.getArguments();
    auto it = args.begin();
    (*it)->accept(*this);
    const auto condition = std::get<double>(value_);
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
      value->accept(*this);
      auto argSymbol = std::make_unique<RuntimeVariableSymbol>(argName, type, value, context_.clone());
      context_.addSymbol(argName, std::move(argSymbol));

      ++it;
    }

    functionAnalyser.getBody()->accept(*this);

    context_.leaveScope();

    if(functionAnalyser.getReturnType() != TypeName::Void)
    {
      value_ = returnStack_.top();
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
  auto const returnedValue = value_;
  returnStack_.push(returnedValue);
}

void Executor::visit(const StringLiteralNode& node)
{
  value_ = node.getValue();
}

void Executor::visit(const UnaryNode& node)
{
  node.getTerm().accept(*this);
  auto term = std::get<double>(value_);

  switch(node.getOperation())
  {
    case UnaryOperator::BinaryNegation:
      value_ = ~static_cast<unsigned int>(term);
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

  std::cout << "Evaluating variable " << name << " in context:\n";
  analyser.getContext().debug();

  value->accept(executor);
  value_ = executor.getValue();
}
