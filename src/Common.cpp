#include "Common.hpp"

#include <stdexcept>

#include "AST.hpp"

[[noreturn]]
void reportError(const std::string& message, const Node& node)
{
  const auto mark = node.getMark();
  std::stringstream ss;
  ss << "ERROR (" << mark.to_string() << "): " << message;
  throw std::runtime_error(ss.str());
}