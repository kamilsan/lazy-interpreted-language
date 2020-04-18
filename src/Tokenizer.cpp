#include "Tokenizer.hpp"

#include <exception>
#include <cstring>
#include <sstream>
#include <set>

const std::set<std::string> Tokenizer::keywords_ = 
  std::set<std::string>{"f32", "if", "print", "fn", "let", "void"};


Tokenizer::Tokenizer(std::istream& stream): stream_(stream), token_(), mark_()
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
        throw tokenizer_exception("Unexpected character!", mark_);
    }
    catch(tokenizer_exception&)
    {
      throw;
    }
  }
  else
  {
    token_ = Token{};
  }

  return token_;
}

bool Tokenizer::tryToSkipComments()
{
  bool result = false;
  while(stream_.peek() == '/')
  {
    if(advance() == '/')
    {
      result = true;
      while(stream_.peek() != '\n' && !stream_.eof())
        advance();
      advance();
    }
    else
    {
      unget();
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
    advance();
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
    advance();
  }
  if(isdigit(stream_.peek()))
  {
    if(stream_.peek() != '0')
    {
      while(isdigit(stream_.peek()))
      {
        result = 10.0 * result + stream_.peek() - '0';
        advance();
      }
    }
    else
    {
      advance();
      if(stream_.peek() != '.')
        throw tokenizer_exception("Unexpected character!", mark_);
    }

    if(stream_.peek() == '.')
    {
      advance();

      float mult = 0.1;
      while(isdigit(stream_.peek()))
      {
        result += (stream_.peek() - '0') * mult;
        mult *= 0.1;
        advance();
      }
    }
  }
  else
  {
    if(sign == -1)
      unget();
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
    advance();
    while(stream_.peek() != '\"')
    {
      if(stream_.peek() == EOF)
        throw tokenizer_exception("Unexpected end of stream!", mark_);
      else if(stream_.peek() == '\\')
      {
        advance();
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
      advance();
    }
    advance();
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
    advance();
    while(isalpha(stream_.peek()) || isdigit(stream_.peek()) || stream_.peek() == '_')
    {
      ss << (char)stream_.peek();
      advance();
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
    case '=':
      token_ = Token{TokenType::AssignOperator, "="};
      break;
    case '%':
      token_ = Token{TokenType::ArithmeticOperator, "%"};
      break;
    case '~':
      token_ = Token{TokenType::BinaryOperator, "~"};
      break;
    case '!':
      token_ = Token{TokenType::LogicalOperator, "!"};
      break;
    default:
      return false;
  }
  advance();
  return true;
}

bool Tokenizer::tryToGetCompoundToken()
{
  const std::set<char> ARITHMETIC_OPS = {'+', '-', '*', '/'};
  const std::set<char> BINARY_OPS = {'^', '&', '|'};

  TokenType type = TokenType::EOT;
  
  if(ARITHMETIC_OPS.find(stream_.peek()) != ARITHMETIC_OPS.end())
    type = TokenType::ArithmeticOperator;
  else if(BINARY_OPS.find(stream_.peek()) != BINARY_OPS.end())
    type = TokenType::BinaryOperator;
  
  if(type != TokenType::EOT)
  {
    char c = stream_.peek();
    advance();
    if(stream_.peek() == '=')
    {
      advance();
      token_ = Token{TokenType::AssignOperator, std::string(1, c) + "="};
    }
    else if(c != '^' && stream_.peek() == c)
    {
      advance();
      token_ = Token{TokenType::LogicalOperator, std::string(2, c)};
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
      throw tokenizer_exception("Unexpected escape sequence!", mark_);
  }
}