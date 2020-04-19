#pragma once

#include <iostream>

#include "Mark.hpp"

class Stream
{
public:
  Stream(std::istream& stream): is_(stream), mark_() {}

  Stream(const Stream&) = delete;
  Stream(Stream&&) = delete;
  Stream& operator=(const Stream&) = delete;
  Stream& operator=(Stream&&) = delete;

  bool eof() const;
  char peek() const;
  const Mark& getMark() const;

  void unget();
  int advance();
private:
  std::istream& is_;
  Mark mark_;
};
