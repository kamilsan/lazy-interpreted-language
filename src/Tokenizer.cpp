#include "Tokenizer.hpp"

#include <cstring>
#include <exception>
#include <unordered_map>
#include <set>
#include <sstream>

const std::set<std::string> Tokenizer::keywords_ = 
  std::set<std::string>{"f32", "if", "print", "fn", "let", "ret", "void", "function"};

const std::unordered_map<std::string, TokenType> Tokenizer::keywordTokenTypes_ = {
  std::make_pair("f32", TokenType::KeywordF32),
  std::make_pair("if", TokenType::KeywordIf),
  std::make_pair("print", TokenType::KeywordPrint),
  std::make_pair("fn", TokenType::KeywordFn),
  std::make_pair("let", TokenType::KeywordLet),
  std::make_pair("ret", TokenType::KeywordRet),
  std::make_pair("void", TokenType::KeywordVoid),
  std::make_pair("function", TokenType::KeywordFunction)
};


std::string Tokenizer::makeErrorMessage(std::string err) const
{
  std::stringstream ss;
  ss << "ERROR: (" << stream_.getMark().to_string() << ") " << err;
  return ss.str();
}


Tokenizer::Tokenizer(std::istream& stream): stream_(stream), token_()
{
  nextToken();
}

bool Tokenizer::end() const
{
  return token_.type == TokenType::EOT;
}

const Token& Tokenizer::peek() const
{
  return token_;
}

Token Tokenizer::nextToken()
{
  if(stream_.peek() != EOF)
  {
    while(tryToSkipSpaces() || tryToSkipComments());

    if(tryToGetKeywordOrIdentifier())
      return token_;
    else if(tryToGetString())
      return token_;
    else if(tryToGetNumber())
      return token_;
    else if(tryToGetSingleCharToken())
      return token_;
    else if(tryToGetCompoundToken())
      return token_;
    else if(stream_.peek() == EOF)
      token_ = Token{};
    else
      throw std::runtime_error(makeErrorMessage("Unexpected character!"));
  }
  else
    token_ = Token{};

  return token_;
}

bool Tokenizer::tryToSkipComments()
{
  bool result = false;
  while(stream_.peek() == '/')
  {
    if(stream_.advance() == '/')
    {
      result = true;
      while(stream_.peek() != '\n' && !stream_.eof())
        stream_.advance();
      stream_.advance();
    }
    else
    {
      stream_.unget();
      break;
    }
  }

  return result;
}

bool Tokenizer::tryToSkipSpaces()
{
  bool result = false;
  while(isspace(stream_.peek()))
  {
    result = true;
    stream_.advance();
  }

  return result;
}

bool Tokenizer::tryToGetNumber()
{
  std::stringstream ss;
  if(isdigit(stream_.peek()))
  {
    if(stream_.peek() != '0')
    {
      while(isdigit(stream_.peek()))
      {
        ss << stream_.peek();
        stream_.advance();
      }
    }
    else
    {
      ss << stream_.peek();
      stream_.advance();
    }

    if(stream_.peek() == '.')
    {
      ss << stream_.peek();
      stream_.advance();
      while(isdigit(stream_.peek()))
      {
        ss << stream_.peek();
        stream_.advance();
      }
    }
    else if(isdigit(stream_.peek()) || isalpha(stream_.peek()))
      throw std::runtime_error(makeErrorMessage("Unexpected character!"));
  }
  else
    return false;

  try
  {
    const double value = std::stod(ss.str());
    token_ = Token(TokenType::Number, value);
  }
  catch(...)
  {
    throw std::runtime_error(makeErrorMessage("Failed to parse numeric constant!"));
  }
  return true;
}

bool Tokenizer::tryToGetString()
{
  std::stringstream ss;
  if(stream_.peek() == '\"')
  {
    stream_.advance();
    while(stream_.peek() != '\"')
    {
      if(stream_.peek() == EOF)
        throw std::runtime_error(makeErrorMessage("Unexpected end of stream!"));

      else if(stream_.peek() == '\\')
      {
        stream_.advance();
        ss << handleEscapeSeqence();
      }
      else
        ss << (char)stream_.peek();
      stream_.advance();
    }
    stream_.advance();
  }
  else
    return false;

  token_ = Token{TokenType::String, ss.str()};
  return true;
}

bool Tokenizer::tryToGetKeywordOrIdentifier()
{
  std::stringstream ss;
  if(isalpha(stream_.peek()) || stream_.peek() == '_')
  {
    ss << (char)stream_.peek();
    stream_.advance();
    while(isalpha(stream_.peek()) || isdigit(stream_.peek()) || stream_.peek() == '_')
    {
      ss << (char)stream_.peek();
      stream_.advance();
    }
  }
  else
    return false;

  const std::string str = ss.str();
  if(keywords_.find(str) != keywords_.end())
  {
    const auto type = keywordTokenTypes_.at(str);
    token_ = Token{type, str};
  }
  else
    token_ = Token{TokenType::Identifier, str};

  return true;
}

bool Tokenizer::tryToGetSingleCharToken()
{
  switch(stream_.peek())
  {
    case ',':
      token_ = Token{TokenType::Comma, ","};
      break;
    case ':':
      token_ = Token{TokenType::Colon, ":"};
      break;
    case ';':
      token_ = Token{TokenType::Semicolon, ";"};
      break;
    case '\\':
      token_ = Token{TokenType::Backslash, "\\"};
      break;
    case '(':
      token_ = Token{TokenType::LParen, "("};
      break;
    case ')':
      token_ = Token{TokenType::RParen, ")"};
      break;
    case '{':
      token_ = Token{TokenType::LBrace, "{"};
      break;
    case '}':
      token_ = Token{TokenType::RBrace, "}"};
      break;
    case '%':
      token_ = Token{TokenType::Modulo, "%"};
      break;
    case '~':
      token_ = Token{TokenType::BinaryNot, "~"};
      break;
    default:
      return false;
  }
  stream_.advance();
  return true;
}

bool Tokenizer::tryToGetCompoundToken()
{
  if(simpleOrWithEq('+', TokenType::Plus, TokenType::PlusEq)          ||
     simpleOrWithEq('-', TokenType::Minus, TokenType::MinusEq)        ||
     simpleOrWithEq('*', TokenType::Mul, TokenType::MulEq)            ||
     simpleOrWithEq('/', TokenType::Div, TokenType::DivEq)            ||
     simpleOrWithEq('!', TokenType::LogicalNot, TokenType::NotEqual)  ||
     simpleOrWithEq('=', TokenType::Assign, TokenType::Equal)         ||
     simpleOrWithEq('^', TokenType::BinaryXor, TokenType::XorEq)      ||
     simpleWithEqOrDouble('&', TokenType::BinaryAnd, TokenType::AndEq, TokenType::LogicalAnd) ||
     simpleWithEqOrDouble('|', TokenType::BinaryOr, TokenType::OrEq, TokenType::LogicalOr)    ||
     comparisonShiftOrAssignment('>', TokenType::Greater, 
      TokenType::GreaterOrEqual, TokenType::ShiftRight, TokenType::ShiftRightEq) ||
     comparisonShiftOrAssignment('<', TokenType::Less, 
      TokenType::LessOrEqual, TokenType::ShiftLeft, TokenType::ShiftLeftEq))
    return true;
  else
    return false;
  
  return true;
}

bool Tokenizer::simpleOrWithEq(char c, TokenType type, TokenType typeEq)
{
  if(stream_.peek() == c)
  {
    if(stream_.advance() == '=')
    {
      token_ = Token{typeEq, std::string(1, c) + "="};
      stream_.advance();
    }
    else
      token_ = Token{type, std::string(1, c)};
  }
  else 
    return false;
  
  return true;
}

bool Tokenizer::simpleWithEqOrDouble(char c, TokenType type, TokenType typeEq, TokenType typeDouble)
{
  if(stream_.peek() == c)
  {
    if(stream_.advance() == '=')
    {
      token_ = Token{typeEq, std::string(1, c) + "="};
      stream_.advance();
    }
    else if(stream_.peek() == c)
    {
      token_ = Token{typeDouble, std::string(2, c)};
      stream_.advance();
    }
    else
      token_ = Token{type, std::string(1, c)};
  }
  else 
    return false;
  
  return true;
}

bool Tokenizer::comparisonShiftOrAssignment(char c, TokenType typeComparison, 
    TokenType typeComparisonEq, TokenType typeShift, TokenType typeAssign)
{
  if(stream_.peek() == c)
  {
    if(stream_.advance() == c)
    {
      if(stream_.advance() == '=')
      {
        token_ = Token{typeAssign, std::string(2, c) + "="};
        stream_.advance();
      }
      else
        token_ = Token{typeShift, std::string(2, c)};
    }
    else if(stream_.peek() == '=')
    {
      token_ = Token{typeComparisonEq, std::string(1, c) + "="};
      stream_.advance();
    }
    else
      token_ = Token{typeComparison, std::string(1, c)};
  }
  else
    return false;

  return true;
}

char Tokenizer::handleEscapeSeqence() const
{
  switch(stream_.peek())
  {
    case '\"':
      return '\"';
    case '\'':
      return '\'';
    case '\\':
      return '\\';
    case '\?':
      return '\?';
    case 'a':
      return '\a';
    case 'b':
      return '\b';
    case 't':
      return '\t';
    case 'v':
      return '\v';
    case 'n':
      return '\n';
    case 'r':
      return '\r';
    case 'f':
      return '\f';
    default:
      throw std::runtime_error(makeErrorMessage("Unexpected escape sequence!"));
  }
}
