#include "Stream.hpp"

bool Stream::eof() const
{
  return is_.eof();
}

int Stream::peek() const
{
  return is_.peek();
}

const Mark& Stream::getMark() const
{
  return mark_;
}

void Stream::unget()
{
  is_.unget();
  mark_.column--;
}

int Stream::advance()
{
  is_.get();
  int c = is_.peek();

  if(c == '\n')
    mark_.newLine();
  else 
    mark_.advance();

  return c;
}