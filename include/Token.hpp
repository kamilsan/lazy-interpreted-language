#pragma once

#include <string>
#include <optional>
#include <vector>

enum class TokenType : int
{
  Identifier = 0,
  Keyword,
  Number,
  String,
  ArithmeticOperator,
  LogicalOperator,
  BinaryOperator,
  AssignOperator,
  ComparisonOperator,
  LParen,
  RParen,
  LBrace,
  RBrace,
  Colon,
  Semicolon,
  Comma,
  Backslash,
  EOT
};

const std::vector<std::string> TOKEN_NAMES = {
  "Identifier",
  "Keyword",
  "Number",
  "String",
  "ArithmeticOperator",
  "LogicalOperator",
  "BinaryOperator",
  "AssignOperator",
  "ComparisonOperator",
  "LParen",
  "RParen",
  "LBrace",
  "RBrace",
  "Colon",
  "Semicolon",
  "Comma",
  "Backslash",
  "EOT"
};

struct Token
{
  Token(): type(TokenType::EOT) {}
  Token(const TokenType& type, std::string value): type(type), stringValue(value) {}
  Token(const TokenType& type, float value): type(type), numericValue(value) {}

  bool operator==(const Token& other) const
  {
    return type == other.type && 
          stringValue == other.stringValue && 
          numericValue == other.numericValue;
  }

  friend std::ostream& operator<<(std::ostream& os, const Token& token) 
  {
    os << TOKEN_NAMES[static_cast<int>(token.type)] << ": ";

    if(token.numericValue.has_value())
      os << token.numericValue.value();
    if(token.stringValue.has_value())
      os << token.stringValue.value();

    return os;
  }

  TokenType type;
  std::optional<std::string> stringValue;
  std::optional<float> numericValue;
};