#include "Parser.hpp"

#include "Node.hpp"

#include <stdexcept>

Parser::Parser(std::istream& stream): tokenizer_(stream) {}

std::unique_ptr<Node> Parser::parseProgram()
{
  return parseAddExpression();
}

std::unique_ptr<Node> Parser::parseAddExpression()
{
  auto factor = parseFactor();
  auto token = tokenizer_.peek();
  if(token.type == TokenType::Plus || token.type == TokenType::Minus ||
    token.type == TokenType::Modulo)
  {
    tokenizer_.nextToken();
    auto left = std::move(factor);
    auto right = parseAddExpression();
    return std::make_unique<BinaryOpNode>(std::move(left), token, std::move(right));
  }
  else
    return factor;
}

std::unique_ptr<Node> Parser::parseFactor()
{
  auto term = parseTerm();
  auto token = tokenizer_.peek();
  if(token.type == TokenType::Mul || token.type == TokenType::Div)
  {
    tokenizer_.nextToken();
    auto left = std::move(term);
    auto right = parseFactor();
    return std::make_unique<BinaryOpNode>(std::move(left), token, std::move(right));
  }
  else
    return term;
}

std::unique_ptr<Node> Parser::parseUnary()
{
  return nullptr;
}

std::unique_ptr<Node> Parser::parseTerm()
{
  auto token = tokenizer_.peek();
  if(token.type == TokenType::Number)
  {
    tokenizer_.nextToken();
    return std::make_unique<LiteralNode>(std::get<double>(token.value));
  }
  return nullptr;
}
