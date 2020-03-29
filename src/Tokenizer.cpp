#include "Tokenizer.hpp"

#include <exception>
#include <cstring>
#include <sstream>

const std::set<std::string> Tokenizer::keywords_ = 
  std::set<std::string>{"f32", "if", "print", "fn", "let", "void"};


Tokenizer::Tokenizer(std::istream& stream): stream_(stream), token_()
{
  try
  {
    nextToken();
  }
  catch(...)
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
      if(tryToGetString())
        return token_;
      else if(stream_.peek() == '-' || isdigit(stream_.peek()))
      {
        if(tryToGetNumber())
          return token_;
      }
    }
    catch(...)
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
    stream_.get();
    if(stream_.peek() == '/')
    {
      result = true;
      while(stream_.peek() != '\n' && !stream_.eof())
        stream_.get();
      stream_.get();
    }
    else
      stream_.unget();
  }

  return result;
}

bool Tokenizer::tryToSkipSpaces()
{
  bool result = false;
  while(isspace(stream_.peek()))
  {
    result = true;
    stream_.get();
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
    stream_.get();
  }
  if(isdigit(stream_.peek()))
  {
    if(stream_.peek() != '0')
    {
      while(isdigit(stream_.peek()))
      {
        result = 10.0 * result + stream_.peek() - '0';
        stream_.get();
      }
    }
    else
    {
      stream_.get();
      if(stream_.peek() != '.')
        throw std::runtime_error("Unexpected character!");
    }

    if(stream_.peek() == '.')
    {
      stream_.get();

      float mult = 0.1;
      while(isdigit(stream_.peek()))
      {
        result += (stream_.peek() - '0') * mult;
        mult *= 0.1;
        stream_.get();
      }
    }
  }
  else if(sign == -1)
  {
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
    stream_.get();
    while(stream_.peek() != '\"')
    {
      if(stream_.peek() == EOF)
        throw std::runtime_error("Unexpected end of stream!");
      else if(stream_.peek() == '\\')
      {
        stream_.get();
        auto c = stream_.peek();
        switch(c)
        {
          case '\"':
            ss << '\"';
            break;
          case '\'':
            ss << '\'';
            break;
          case '\\':
            ss << '\\';
            break;
          case '\?':
            ss << '\?';
            break;
          case 'a':
            ss << '\a';
            break;
          case 'b':
            ss << '\b';
            break;
          case 't':
            ss << '\t';
            break;
          case 'v':
            ss << '\v';
            break;
          case 'n':
            ss << '\n';
            break;
          case 'r':
            ss << '\r';
            break;
          case 'f':
            ss << '\f';
            break;
          default:
            throw std::runtime_error("Unexpected escape sequence!");
            break; 
        }
      }
      else
        ss << (char)stream_.peek();
      stream_.get();
    }
    stream_.get();
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
    stream_.get();
    while(isalpha(stream_.peek()) || isdigit(stream_.peek()) || stream_.peek() == '_')
    {
      ss << (char)stream_.peek();
      stream_.get();
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