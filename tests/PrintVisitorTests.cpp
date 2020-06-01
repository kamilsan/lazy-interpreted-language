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

TEST(PrintVisitorTest, StringLiteral)
{
  std::stringstream result;
  PrintVisitor visitor{result};
  auto node = std::make_unique<StringLiteralNode>("test");
  node->accept(visitor);

  std::string expected{"StringLiteral: test\n"};
  EXPECT_EQ(result.str(), expected);
}

TEST(PrintVisitorTest, BinaryOperation)
{
  std::stringstream result;
  PrintVisitor visitor{result};
  auto node = std::make_unique<BinaryOpNode>(std::make_unique<NumericLiteralNode>(1), BinaryOperator::Addition, std::make_unique<NumericLiteralNode>(2));
  node->accept(visitor);

  std::string expected{"BinaryOpNode:\n NumericLiteral: 1\nAddition\n NumericLiteral: 2\n"};
  EXPECT_EQ(result.str(), expected);
}
