#pragma once

struct Mark
{
  Mark(): line(1), column(0) {}
  Mark(int ln, int col): line(ln), column(col) {}

  void advance() { column++; }
  void newLine() { line++; column = 0; }

  int line;
  int column;
};