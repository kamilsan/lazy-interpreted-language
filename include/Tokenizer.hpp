#pragma once

#include <iostream>
#include <set>

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

  const static std::set<std::string> keywords_;

  bool tryToSkipComments();
  bool tryToSkipSpaces();
  bool tryToGetNumber();
  bool tryToGetKeywordOrIdentifier();
};