#include <fstream>
#include <iostream>

#include "Parser.hpp"
#include "PrintVisitor.hpp"
#include "SemanticAnalyser.hpp"
#include "Executor.hpp"

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
    if(!sourceFile.is_open())
    {
      std::cout << "Could not open provided source file!\n";
      return 0;
    }

    Parser parser{sourceFile};
    SemanticAnalyser semantic{};
    Executor executor{};
    
    auto program = parser.parseBlock();
    program->accept(semantic);
    program->accept(executor);
    
    sourceFile.close();
  }
  catch(std::runtime_error& er)
  {
    std::cout << er.what() << "\n";
  }

  return 0;
}
