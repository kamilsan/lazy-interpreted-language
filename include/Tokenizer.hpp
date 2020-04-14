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
  const static std::set<std::string> keywords_;

  std::istream& stream_;
  Token token_;

  bool tryToSkipComments();
  bool tryToSkipSpaces();
  bool tryToGetNumber();
  bool tryToGetString();
  bool tryToGetKeywordOrIdentifier();
  bool tryToGetSingleCharToken();
  bool tryToGetCompoundToken();

  char handleEscapeSeqence() const;
};