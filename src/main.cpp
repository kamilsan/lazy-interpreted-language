#include <fstream>
#include <iostream>

#include "Node.hpp"
#include "Parser.hpp"
#include "PrintVisitor.hpp"

int main(int argc, char* argv[])
{
  if(argc != 2)
  {
    std::cout << "Usage: " << argv[0] << " source_file\n";
    return 0;
  }

  try
  {
    std::ifstream sourceFile{argv[1]};
    Parser parser{sourceFile};
    PrintVisitor visitor{};
    parser.parseProgram()->accept(visitor);
  }
  catch(std::runtime_error& er)
  {
    std::cout << er.what() << "\n";
  }

  return 0;
}
