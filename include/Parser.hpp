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
  std::unique_ptr<StatementNode> parseVariableDeclaration();
  std::unique_ptr<StatementNode> parseReturnStatement();

private:
  Tokenizer tokenizer_;

  void expectToken(TokenType type, const std::string& msg);
  Token getToken(TokenType type, const std::string& msg);

  UnaryOperation unaryOperationFromToken(const Token& token) const;
  BinaryOperation binaryOperationFromToken(const Token& token) const;
};
