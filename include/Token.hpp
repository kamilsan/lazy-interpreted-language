#pragma once

#include <string>
#include <optional>

enum class TokenType
{
  Identifier,
  Keyword,
  Number,
  String,
  ArithmeticOpeartor,
  LogicalOperator,
  BinaryOperator,
  LParen,
  RParen,
  Semicolon,
  Backslash,
  EOT
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

  TokenType type;
  std::optional<std::string> stringValue;
  std::optional<float> numericValue;
};