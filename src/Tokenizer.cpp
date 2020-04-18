#include "Tokenizer.hpp"

#include <exception>
#include <cstring>
#include <sstream>
#include <set>

const std::set<std::string> Tokenizer::keywords_ = 
  std::set<std::string>{"f32", "if", "print", "fn", "let", "void"};


Tokenizer::Tokenizer(std::istream& stream): stream_(stream), token_()
{
  try
  {
    nextToken();
  }
  catch(std::runtime_error&)
  {
    throw;
  }
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
    try
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
        throw tokenizer_exception("Unexpected character!", stream_.getMark());
    }
    catch(tokenizer_exception&)
    {
      throw;
    }
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
  int sign = 1;
  float result = 0;
  if(stream_.peek() == '-')
  {
    sign = -1;
    stream_.advance();
  }
  if(isdigit(stream_.peek()))
  {
    if(stream_.peek() != '0')
    {
      while(isdigit(stream_.peek()))
      {
        result = 10.0 * result + stream_.peek() - '0';
        stream_.advance();
      }
    }
    else
    {
      stream_.advance();
      if(stream_.peek() != '.')
        throw tokenizer_exception("Unexpected character!", stream_.getMark());
    }

    if(stream_.peek() == '.')
    {
      stream_.advance();

      float mult = 0.1;
      while(isdigit(stream_.peek()))
      {
        result += (stream_.peek() - '0') * mult;
        mult *= 0.1;
        stream_.advance();
      }
    }
  }
  else
  {
    if(sign == -1)
      stream_.unget();
    return false;
  }

  token_ = Token(TokenType::Number, sign * result);
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
        throw tokenizer_exception("Unexpected end of stream!", stream_.getMark());
      else if(stream_.peek() == '\\')
      {
        stream_.advance();
        try
        {
          ss << handleEscapeSeqence();
        }
        catch(tokenizer_exception&)
        {
          throw;
        }
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

  std::string str = ss.str();
  if(keywords_.find(str) != keywords_.end())
    token_ = Token{TokenType::Keyword, str};
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
     simpleWithEqOrDouble('|', TokenType::BinaryOr, TokenType::OrEq, TokenType::LogicalOr))
    return true;
  else if(stream_.peek() == '>')
  {
    if(stream_.advance() == '>')
    {
      if(stream_.advance() == '=')
      {
        token_ = Token{TokenType::ShiftRightEq, ">>="};
        stream_.advance();
      }
      else
        token_ = Token{TokenType::ShiftRight, ">>"};
    }
    else if(stream_.peek() == '=')
    {
      token_ = Token{TokenType::GreaterOrEqual, ">="};
      stream_.advance();
    }
    else
      token_ = Token{TokenType::Greater, ">"};
  }
  else if(stream_.peek() == '<')
  {
    if(stream_.advance() == '<')
    {
      if(stream_.advance() == '=')
      {
        token_ = Token{TokenType::ShiftLeftEq, "<<="};
        stream_.advance();
      }
      else
        token_ = Token{TokenType::ShiftLeft, "<<"};
    }
    else if(stream_.peek() == '=')
    {
      stream_.advance();
      token_ = Token{TokenType::LessOrEqual, "<="};
    }
    else
      token_ = Token{TokenType::Less, "<"};
  }
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
      throw tokenizer_exception("Unexpected escape sequence!", stream_.getMark());
  }
}