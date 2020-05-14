#include <gtest/gtest.h>
#include <sstream>

#include "AST.hpp"
#include "PrintVisitor.hpp"

TEST(PrintVisitorTest, NumericLiteral)
{
  std::stringstream result;
  PrintVisitor visitor{result};
  auto node = std::make_unique<NumericLiteralNode>(10);
  node->accept(visitor);

  std::string expected{"NumericLiteral: 10\n"};
  EXPECT_EQ(result.str(), expected);
}

// To be continued...