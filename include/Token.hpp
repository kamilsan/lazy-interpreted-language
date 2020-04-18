#pragma once

#include <optional>
#include <string>
#include <vector>

enum class TokenType : int
{
  Identifier = 0,
  Keyword,
  Number,
  String,
  Plus,
  Minus,
  Mul,
  Div,
  Modulo,
  Assign,
  LogicalAnd,
  LogicalOr,
  LogicalNot,
  Greater,
  Less,
  GreaterOrEqual,
  LessOrEqual,
  Equal,
  NotEqual,
  ShiftLeft,
  ShiftRight,
  BinaryAnd,
  BinaryOr,
  BinaryNot,
  BinaryXor,
  PlusEq,
  MinusEq,
  MulEq,
  DivEq,
  AndEq,
  OrEq,
  XorEq,
  ShiftLeftEq,
  ShiftRightEq,
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
  "Plus",
  "Minus",
  "Mul",
  "Div",
  "Modulo",
  "Assign",
  "LogicalAnd",
  "LogicalOr",
  "LogicalNot",
  "Greater",
  "Less",
  "GreaterOrEqual",
  "LessOrEqual",
  "Equal",
  "NotEqual",
  "ShiftLeft",
  "ShiftRight",
  "BinaryAnd",
  "BinaryOr",
  "BinaryNot",
  "BinaryXor",
  "PlusEq",
  "MinusEq",
  "MulEq",
  "DivEq",
  "AndEq",
  "OrEq",
  "XorEq",
  "ShiftLeftEq",
  "ShiftRightEq",
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

  static bool isAssigmentOperator(const Token& token);
  static bool isArithmeticOperator(const Token& token);
  static bool isLogicalOperator(const Token& token);
  static bool isBinaryOperator(const Token& token);
  static bool isComparisonOperator(const Token& token);

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

inline bool Token::isAssigmentOperator(const Token& token)
{
  auto type = token.type;
  return type == TokenType::Assign || type == TokenType::PlusEq ||
    type == TokenType::MinusEq || type == TokenType::MulEq ||
    type == TokenType::DivEq || type == TokenType::AndEq ||
    type == TokenType::OrEq || type == TokenType::XorEq ||
    type == TokenType::ShiftLeftEq || type == TokenType::ShiftRightEq;
}

inline bool Token::isArithmeticOperator(const Token& token)
{
  auto type = token.type;
  return type == TokenType::Plus || type == TokenType::Minus ||
    type == TokenType::Mul || type == TokenType::Div ||
    type == TokenType::Modulo;
}

inline bool Token::isLogicalOperator(const Token& token)
{
  auto type = token.type;
  return type == TokenType::LogicalAnd || type == TokenType::LogicalOr ||
    type == TokenType::LogicalNot;
}

inline bool Token::isBinaryOperator(const Token& token)
{
  auto type = token.type;
  return type == TokenType::BinaryAnd || type == TokenType::BinaryOr ||
    type == TokenType::BinaryNot || type == TokenType::BinaryXor ||
    type == TokenType::ShiftLeft || type == TokenType::ShiftRight;
}

inline bool Token::isComparisonOperator(const Token& token)
{
  auto type = token.type;
  return type == TokenType::Equal || type == TokenType::NotEqual ||
    type == TokenType::Greater || type == TokenType::Less ||
    type == TokenType::GreaterOrEqual || type == TokenType::LessOrEqual;
}
