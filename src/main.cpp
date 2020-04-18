#include <iostream>
#include <fstream>

#include "Tokenizer.hpp"

int main(int argc, char* argv[])
{
  if(argc != 2)
  {
    std::cout << "Usage: " << argv[0] << " source_file\n";
    return 0;
  }

  std::ifstream file(argv[1]);
  if(!file.good())
  {
    std::cout << "ERROR: Could not open provided input file!\n";
    return -1;
  }

  try
  {
    Tokenizer tokenizer{file};
    std::cout << "TOKENS:\n";
    while(!tokenizer.end())
    {
      auto token = tokenizer.peek();
      std::cout << token << "\n";
      tokenizer.nextToken();
    }
  }
  catch(tokenizer_exception& er)
  {
    std::cout << er.what() << "\n";
  }
  file.close();
  return 0;
}