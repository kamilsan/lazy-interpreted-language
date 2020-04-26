#pragma once

#include <sstream>

struct Mark
{
  Mark(): line(1), column(0) {}
  Mark(int ln, int col): line(ln), column(col) {}

  std::string to_string() const
  {
    std::stringstream ss;
    ss << "Ln: " << line << ", Col: " << column;
    return ss.str();
  }

  void advance() { column++; }
  void newLine() { line++; column = 0; }

  int line;
  int column;
};