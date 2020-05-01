#include <fstream>
#include <iostream>

#include "Parser.hpp"
#include "Node.hpp"
#include "PrintVisitor.hpp"

int main()
{
  try
  {
    std::stringstream ss{"2 + 3 * -x / 5 - (1 ^ 3)"};
    Parser parser{ss};
    PrintVisitor visitor{};
    parser.parseProgram()->accept(visitor);
  }
  catch(std::runtime_error& er)
  {
    std::cout << er.what() << "\n";
  }

  return 0;
}
