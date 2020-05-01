#pragma once

#include <memory>

#include "Tokenizer.hpp"
#include "Node.hpp"

class Parser
{
public:
  Parser(std::istream& stream);

  std::unique_ptr<Node> parseProgram();
  std::unique_ptr<ExpressionNode> parseArithmeticExpression();
  std::unique_ptr<ExpressionNode> parseAddExpression();
  std::unique_ptr<ExpressionNode> parseFactor();
  std::unique_ptr<ExpressionNode> parseUnary();
  std::unique_ptr<ExpressionNode> parseTerm();

private:
  Tokenizer tokenizer_;

  UnaryOperation unaryOperationFromToken(const Token& token) const;
  BinaryOperation binaryOperationFromToken(const Token& token) const;
};
