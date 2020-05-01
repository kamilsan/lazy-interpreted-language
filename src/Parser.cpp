#include "Parser.hpp"

#include "Node.hpp"

#include <stdexcept>

Parser::Parser(std::istream& stream): tokenizer_(stream) {}

std::unique_ptr<Node> Parser::parseProgram()
{
  return parseArithmeticExpression();
}

std::unique_ptr<ExpressionNode> Parser::parseArithmeticExpression()
{
  auto addExpr = parseAddExpression();
  auto token = tokenizer_.peek();
  if(Token::isBinaryOperator(token))
  {
    tokenizer_.nextToken();
    auto left = std::move(addExpr);
    auto right = parseArithmeticExpression();
    auto op = binaryOperationFromToken(token);
    return std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
  }
  else
    return addExpr;
}

std::unique_ptr<ExpressionNode> Parser::parseAddExpression()
{
  auto factor = parseFactor();
  auto token = tokenizer_.peek();
  if(token.type == TokenType::Plus || token.type == TokenType::Minus ||
    token.type == TokenType::Modulo)
  {
    tokenizer_.nextToken();
    auto left = std::move(factor);
    auto right = parseAddExpression();
    auto op = binaryOperationFromToken(token);
    return std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
  }
  else
    return factor;
}

std::unique_ptr<ExpressionNode> Parser::parseFactor()
{
  auto unary = parseUnary();
  auto token = tokenizer_.peek();
  if(token.type == TokenType::Mul || token.type == TokenType::Div)
  {
    tokenizer_.nextToken();
    auto left = std::move(unary);
    auto right = parseFactor();
    auto op = binaryOperationFromToken(token);
    return std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
  }
  else
    return unary;
}

std::unique_ptr<ExpressionNode> Parser::parseUnary()
{
  auto token = tokenizer_.peek();
  if(token.type == TokenType::Minus || token.type == TokenType::BinaryNot)
  {
    tokenizer_.nextToken();
    auto op = unaryOperationFromToken(token);
    auto term = parseTerm();
    return std::make_unique<UnaryNode>(op, std::move(term));
  }
  else
    return parseTerm();
}

std::unique_ptr<ExpressionNode> Parser::parseTerm()
{
  auto token = tokenizer_.peek();
  if(token.type == TokenType::Number)
  {
    tokenizer_.nextToken();
    return std::make_unique<LiteralNode>(std::get<double>(token.value));
  }
  else if(token.type == TokenType::Identifier)
  {
    tokenizer_.nextToken();
    return std::make_unique<VariableNode>(std::get<std::string>(token.value));
  }
  else if(token.type == TokenType::LParen)
  {
    tokenizer_.nextToken();
    auto expr = parseArithmeticExpression();
    if(tokenizer_.peek().type == TokenType::RParen)
    {
      tokenizer_.nextToken();
      return expr;
    }
    else
      throw std::runtime_error("Expected closing parenthesis!");
  }
  return nullptr;
}

UnaryOperation Parser::unaryOperationFromToken(const Token& token) const
{
  auto type = token.type;
  switch(type)
  {
    case TokenType::Minus:
      return UnaryOperation::Minus;
    case TokenType::BinaryNot:
      return UnaryOperation::BinaryNegation;
    default:
      throw std::runtime_error("Unexpected token type!");
  }
}

BinaryOperation Parser::binaryOperationFromToken(const Token& token) const
{
  auto type = token.type;
  switch(type)
  {
    case TokenType::Plus:
      return BinaryOperation::Addition;
    case TokenType::Minus:
      return BinaryOperation::Subtraction;
    case TokenType::Mul:
      return BinaryOperation::Multiplication;
    case TokenType::Div:
      return BinaryOperation::Division;
    case TokenType::Modulo:
      return BinaryOperation::Modulo;
    case TokenType::LogicalAnd:
      return BinaryOperation::LogicalAnd;
    case TokenType::LogicalOr:
      return BinaryOperation::LogicalOr;
    case TokenType::BinaryAnd:
      return BinaryOperation::BinaryAnd;
    case TokenType::BinaryOr:
      return BinaryOperation::BinaryOr;
    case TokenType::BinaryXor:
      return BinaryOperation::BinaryXor;
    case TokenType::ShiftLeft:
      return BinaryOperation::ShiftLeft;
    case TokenType::ShiftRight:
      return BinaryOperation::ShiftRight;
    case TokenType::Greater:
      return BinaryOperation::Greater;
    case TokenType::GreaterOrEqual:
      return BinaryOperation::GreaterEq;
    case TokenType::Less:
      return BinaryOperation::Less;
    case TokenType::LessOrEqual:
      return BinaryOperation::LessEq;
    case TokenType::Equal:
      return BinaryOperation::Equal;
    case TokenType::NotEqual:
      return BinaryOperation::NotEqual;
    default:
      throw std::runtime_error("Unexpected token type!");
  }
}

