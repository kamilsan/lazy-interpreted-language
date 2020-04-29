#pragma once

#include <memory>

#include "Tokenizer.hpp"

class Node;

class Parser
{
public:
  Parser(std::istream& stream);

  std::unique_ptr<Node> parseProgram();
  std::unique_ptr<Node> parseAddExpression();
  std::unique_ptr<Node> parseFactor();
  std::unique_ptr<Node> parseUnary();
  std::unique_ptr<Node> parseTerm();

private:
  Tokenizer tokenizer_;
};
