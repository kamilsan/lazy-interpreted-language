#include "Executor.hpp"

#include <cmath>
#include <iostream>

#include "Common.hpp"
#include "AST.hpp"

void Executor::assertValueType(const Value& value, const TypeName& type, const std::string& activity, const Node& node) const
{
  if(value.getType() != type)
    reportError("Cannot perform " + activity +
      " with value of type " + TypeNameStrings.at(value.getType()) + "!", node);
}

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

    assertValueType(*executor.getValue(), TypeName::F32,
                    "assignment operation " + AssignmentOperationNames.at(node.getOperation()), node);

    executor.getValue()->accept(valueAnalyser);
    const auto oldValue = valueAnalyser.getValue().value();

    node.getValue()->accept(*this);

    assertValueType(*value_, TypeName::F32,
      "assignment operation " + AssignmentOperationNames.at(node.getOperation()), node);

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
        reportError("String cannot be concatenated with value of type "  +
          TypeNameStrings.at(right->getType()) + "!", node);
    }
    else if(left->getType() == TypeName::F32)
    {
      NumberValueAnalyser numberValueAnalyser{};
      left->accept(numberValueAnalyser);
      const auto l = numberValueAnalyser.getValue().value();

      assertValueType(*right, TypeName::F32, "addition", node);

      right->accept(numberValueAnalyser);
      const auto r = numberValueAnalyser.getValue().value();

      value_ = std::make_unique<Number>(l + r);
    }
    else
      reportError("Operation cannot be performed with value of type "  +
                  TypeNameStrings.at(left->getType()) + "!", node);
  }
  else {
    assertValueType(*left, TypeName::F32,
            "binary operation " + BinaryOperationNames.at(node.getOperation()), node);
    assertValueType(*right, TypeName::F32,
            "binary operation " + BinaryOperationNames.at(node.getOperation()), node);

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
    handlePrint(node);
  else if(name == "if")
    handleIf(node);
  else
  {
    const auto symbol = context_.lookup(name);
    auto functionAnalyser = RuntimeFunctionAnalyser{};
    symbol.value().get().accept(functionAnalyser);

    if(functionAnalyser.isSymbolValid())
    {
      handleFunctionCall(node, functionAnalyser);
    }
    else
    {
      auto variableAnalyser = RuntimeVariableAnalyser{};
      symbol.value().get().accept(variableAnalyser);
      handleVariableCall(node, variableAnalyser);
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

void Executor::visit(const FunctionResultCallNode& node)
{
  node.getCall().accept(*this);
  assertValueType(*value_, TypeName::Function, "function call", node);

  callValue(node, "result", *value_);
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

void Executor::visit(const LambdaNode& node)
{
  value_ = std::make_unique<Function>(node.getReturnType(),
            node.getArguments(), node.getBodyPtr(), context_.clone());
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
  auto returnedValue = value_->clone();
  returnStack_.emplace(std::move(returnedValue));
}

void Executor::visit(const StringLiteralNode& node)
{
  value_ = std::make_unique<String>(node.getValue());
}

void Executor::visit(const UnaryNode& node)
{
  node.getTerm().accept(*this);

  assertValueType(*value_, TypeName::F32,
    "unary operation " + UnaryOperationNames.at(node.getOperation()), node);

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

  if(analyser.isSymbolValid())
  {
    const auto &value = analyser.getValue();
    auto executor = Executor{analyser.getContext()};

    value->accept(executor);
    value_ = executor.getValue()->clone();
  }
  else
  {
    RuntimeFunctionAnalyser functionAnalyser{};
    symbol.value().get().accept(functionAnalyser);

    const auto returnType = functionAnalyser.getReturnType().value();
    const auto args = functionAnalyser.getArguments();
    const auto body = functionAnalyser.getBody();

    value_ = std::make_unique<Function>(returnType, args, body, context_.clone());
  }
}

void Executor::handlePrint(const FunctionCallNode& node)
{
  const auto& args = node.getArguments();
  (*args.begin())->accept(*this);

  if(value_->getType() != TypeName::String)
    reportError("Function print expected string, but got " +
      TypeNameStrings.at(value_->getType()) + "!", node);

  StringValueAnalyser analyser{};
  value_->accept(analyser);

  const auto str = analyser.getValue().value();
  std::cout << str << "\n";
}

void Executor::handleIf(const FunctionCallNode& node)
{
  const auto& args = node.getArguments();
  auto it = args.begin();
  (*it)->accept(*this);

  if(value_->getType() != TypeName::F32)
    reportError("Function if expected logical expression, but got " +
                TypeNameStrings.at(value_->getType()) + "!", node);

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

void Executor::handleVariableCall(const FunctionCallNode& node, const RuntimeVariableAnalyser& variableAnalyser)
{
  const auto value = variableAnalyser.getValue();
  auto executor = Executor{variableAnalyser.getContext()};
  value->accept(executor);

  callValue(node, node.getName(), *executor.getValue());
}

void Executor::handleFunctionCall(const FunctionCallNode& node, const RuntimeFunctionAnalyser& functionAnalyser)
{
  context_.enterScope();

  auto it = node.getArguments().begin();
  for (const auto &arg : functionAnalyser.getArguments())
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

  if (functionAnalyser.getReturnType() != TypeName::Void)
  {
    value_ = std::move(returnStack_.top());
    returnStack_.pop();
  }
}

void Executor::callValue(const CallNode& node, const std::string& name, const Value& value)
{
  auto valueAnalyser = FunctionValueAnalyser{};
  value.accept(valueAnalyser);

  const auto nExpectedArgs = valueAnalyser.getArguments()->size();
  const auto nProvidedArgs = node.getArguments().size();
  if(nExpectedArgs != nProvidedArgs)
    reportError("Function " + name + " expected " +
                std::to_string(nExpectedArgs) + ", but got " + std::to_string(nProvidedArgs) + " arguments!", node);

  Context newContext = valueAnalyser.getContext()->clone();

  newContext.enterScope();

  auto it = node.getArguments().begin();
  for (const auto &arg : valueAnalyser.getArguments().value())
  {
    const auto argName = arg.first;
    const auto type = arg.second;
    std::shared_ptr<ExpressionNode> argValue = *it;
    auto argSymbol = std::make_unique<RuntimeVariableSymbol>(argName, type, argValue, newContext.clone());
    newContext.addSymbol(argName, std::move(argSymbol));

    ++it;
  }

  Executor functionExecutor{newContext};
  valueAnalyser.getBody()->accept(functionExecutor);

  newContext.leaveScope();

  if (valueAnalyser.getReturnType() != TypeName::Void)
  {
    value_ = functionExecutor.getValue()->clone();
  }
}