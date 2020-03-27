#pragma once

#include <iostream>

#include "Token.hpp"

class Tokenizer
{
public:
  Tokenizer(std::istream& stream);

  bool end() const;
  const Token& peek() const;
  Token nextToken();
private:
  std::istream& stream_;
  Token token_;

  bool tryToSkipComments();
  bool tryToSkipSpaces();
  bool tryToGetNumber();
  bool tryToGetKeywordOrIdentifier();
};