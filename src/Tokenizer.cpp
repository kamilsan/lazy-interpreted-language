#include "Tokenizer.hpp"

#include <cstring>

Tokenizer::Tokenizer(std::istream& stream): stream_(stream)
{
  nextToken();
}

bool Tokenizer::end() const
{
  return stream_.eof();
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

    if(isalpha(stream_.peek()) && tryToGetKeywordOrIdentifier())
      return token_;
    else if(stream_.peek() == '-' || isdigit(stream_.peek()))
    {
      if(tryToGetNumber())
        return token_;
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
    while(isdigit(stream_.peek()))
    {
      result = 10.0 * result + stream_.peek() - '0';
      stream_.get();
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

bool Tokenizer::tryToGetKeywordOrIdentifier()
{
  return false;
}