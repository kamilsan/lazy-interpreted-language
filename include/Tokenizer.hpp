#pragma once

#include <set>
#include <string>
#include <unordered_map>

#include "Stream.hpp"
#include "Token.hpp"

class Tokenizer
{
public:
  Tokenizer(std::istream& stream);

  bool end() const;
  const Token& peek() const;
  const Mark& getMark() const { return stream_.getMark(); }
  Token nextToken();
private:
  const static std::set<std::string> keywords_;
  const static std::unordered_map<std::string, TokenType> keywordTokenTypes_;

  Stream stream_;
  Token token_;

  std::string makeErrorMessage(std::string err) const;

  bool tryToSkipComments();
  bool tryToSkipSpaces();
  bool tryToGetNumber();
  bool tryToGetString();
  bool tryToGetKeywordOrIdentifier();
  bool tryToGetSingleCharToken();
  bool tryToGetCompoundToken();

  bool simpleOrWithEq(char c, TokenType type, TokenType typeEq);
  bool simpleWithEqOrDouble(char c, TokenType type, TokenType typeEq, TokenType typeDouble);
  bool comparisonShiftOrAssignment(char c, TokenType typeComparison, 
    TokenType typeComparisonEq, TokenType typeShift, TokenType typeAssign);

  char handleEscapeSeqence() const;
};
