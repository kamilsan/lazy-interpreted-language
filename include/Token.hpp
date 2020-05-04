#pragma once

#include <unordered_map>
#include <string>
#include <variant>
#include <vector>

enum class TokenType
{
  Identifier,
  KeywordIf,
  KeywordPrint,
  KeywordLet,
  KeywordRet,
  KeywordFn,
  KeywordF32,
  KeywordVoid,
  KeywordFunction,
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

const std::unordered_map<TokenType, std::string> TOKEN_NAMES = {
  std::make_pair(TokenType::Identifier, "Identifier"),
  std::make_pair(TokenType::KeywordIf, "KeywordIf"),
  std::make_pair(TokenType::KeywordPrint, "KeywordPrint"),
  std::make_pair(TokenType::KeywordLet, "KeywordLet"),
  std::make_pair(TokenType::KeywordRet, "KeywordRet"),
  std::make_pair(TokenType::KeywordFn, "KeywordFn"),
  std::make_pair(TokenType::KeywordF32, "KeywordF32"),
  std::make_pair(TokenType::KeywordVoid, "KeywordVoid"),
  std::make_pair(TokenType::KeywordFunction, "KeywordFunction"),
  std::make_pair(TokenType::Number, "Number"),
  std::make_pair(TokenType::String, "String"),
  std::make_pair(TokenType::Plus, "Plus"),
  std::make_pair(TokenType::Minus, "Minus"),
  std::make_pair(TokenType::Mul, "Mul"),
  std::make_pair(TokenType::Div, "Div"),
  std::make_pair(TokenType::Modulo, "Modulo"),
  std::make_pair(TokenType::Assign, "Assign"),
  std::make_pair(TokenType::LogicalAnd, "LogicalAnd"),
  std::make_pair(TokenType::LogicalOr, "LogicalOr"),
  std::make_pair(TokenType::LogicalNot, "LogicalNot"),
  std::make_pair(TokenType::Greater, "Greater"),
  std::make_pair(TokenType::Less, "Less"),
  std::make_pair(TokenType::GreaterOrEqual, "GreaterOrEqual"),
  std::make_pair(TokenType::LessOrEqual, "LessOrEqual"),
  std::make_pair(TokenType::Equal, "Equal"),
  std::make_pair(TokenType::NotEqual, "NotEqual"),
  std::make_pair(TokenType::ShiftLeft, "ShiftLeft"),
  std::make_pair(TokenType::ShiftRight, "ShiftRight"),
  std::make_pair(TokenType::BinaryAnd, "BinaryAnd"),
  std::make_pair(TokenType::BinaryOr, "BinaryOr"),
  std::make_pair(TokenType::BinaryNot, "BinaryNot"),
  std::make_pair(TokenType::BinaryXor, "BinaryXor"),
  std::make_pair(TokenType::PlusEq, "PlusEq"),
  std::make_pair(TokenType::MinusEq, "MinusEq"),
  std::make_pair(TokenType::MulEq, "MulEq"),
  std::make_pair(TokenType::DivEq, "DivEq"),
  std::make_pair(TokenType::AndEq, "AndEq"),
  std::make_pair(TokenType::OrEq, "OrEq"),
  std::make_pair(TokenType::XorEq, "XorEq"),
  std::make_pair(TokenType::ShiftLeftEq, "ShiftLeftEq"),
  std::make_pair(TokenType::ShiftRightEq, "ShiftRightEq"),
  std::make_pair(TokenType::LParen, "LParen"),
  std::make_pair(TokenType::RParen, "RParen"),
  std::make_pair(TokenType::LBrace, "LBrace"),
  std::make_pair(TokenType::RBrace, "RBrace"),
  std::make_pair(TokenType::Colon, "Colon"),
  std::make_pair(TokenType::Semicolon, "Semicolon"),
  std::make_pair(TokenType::Comma, "Comma"),
  std::make_pair(TokenType::Backslash, "Backslash"),
  std::make_pair(TokenType::EOT, "EOT")
};

struct Token
{
  Token(): type(TokenType::EOT) {}
  Token(const TokenType& type, std::string value): type(type), value(value) {}
  Token(const TokenType& type, double value): type(type), value(value) {}

  bool operator==(const Token& other) const
  {
    return type == other.type && value == other.value;
  }

  static bool isAssigmentOperator(const Token& token);
  static bool isArithmeticOperator(const Token& token);
  static bool isLogicalOperator(const Token& token);
  static bool isBinaryOperator(const Token& token);
  static bool isComparisonOperator(const Token& token);
  static bool isTypeName(const Token& token);

  friend std::ostream& operator<<(std::ostream& os, const Token& token) 
  {
    os << TOKEN_NAMES.at(token.type) << ": ";

    if(std::holds_alternative<std::string>(token.value))
      os << std::get<std::string>(token.value);
    if(std::holds_alternative<double>(token.value))
      os << std::get<double>(token.value);
      
    return os;
  }

  TokenType type;
  std::variant<std::string, double> value;
};

inline bool Token::isAssigmentOperator(const Token& token)
{
  const auto type = token.type;
  return type == TokenType::Assign || type == TokenType::PlusEq ||
    type == TokenType::MinusEq || type == TokenType::MulEq ||
    type == TokenType::DivEq || type == TokenType::AndEq ||
    type == TokenType::OrEq || type == TokenType::XorEq ||
    type == TokenType::ShiftLeftEq || type == TokenType::ShiftRightEq;
}

inline bool Token::isArithmeticOperator(const Token& token)
{
  const auto type = token.type;
  return type == TokenType::Plus || type == TokenType::Minus ||
    type == TokenType::Mul || type == TokenType::Div ||
    type == TokenType::Modulo;
}

inline bool Token::isLogicalOperator(const Token& token)
{
  const auto type = token.type;
  return type == TokenType::LogicalAnd || type == TokenType::LogicalOr ||
    type == TokenType::LogicalNot;
}

inline bool Token::isBinaryOperator(const Token& token)
{
  const auto type = token.type;
  return type == TokenType::BinaryAnd || type == TokenType::BinaryOr ||
    type == TokenType::BinaryNot || type == TokenType::BinaryXor ||
    type == TokenType::ShiftLeft || type == TokenType::ShiftRight;
}

inline bool Token::isComparisonOperator(const Token& token)
{
  const auto type = token.type;
  return type == TokenType::Equal || type == TokenType::NotEqual ||
    type == TokenType::Greater || type == TokenType::Less ||
    type == TokenType::GreaterOrEqual || type == TokenType::LessOrEqual;
}

inline bool Token::isTypeName(const Token& token)
{
  const auto type = token.type;
  return type == TokenType::KeywordF32 || type == TokenType::KeywordFunction;
}