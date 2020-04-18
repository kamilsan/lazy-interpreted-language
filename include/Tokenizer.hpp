#pragma once

#include <set>
#include <stdexcept>
#include <sstream>

#include "Stream.hpp"
#include "Token.hpp"

class tokenizer_exception : public std::exception
{
public:
  tokenizer_exception(const std::string& msg, const Mark& mark)
  {
    std::stringstream ss;
    ss << "ERROR: (Ln " << mark.line << ", Col " << mark.column << ") " << msg;
    msg_ = ss.str();
  }

	const char* what() const throw()
  {
    return msg_.c_str();
  }
private:
  std::string msg_;
};

class Tokenizer
{
public:
  Tokenizer(std::istream& stream);

  bool end() const;
  const Token& peek() const;
  Token nextToken();
private:
  const static std::set<std::string> keywords_;

  Stream stream_;
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