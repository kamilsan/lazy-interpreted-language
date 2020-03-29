#include <gtest/gtest.h>
#include <sstream>

#include "Tokenizer.hpp"

TEST(TokenizerTest, EmptySource)
{
  std::stringstream stream{""};
  Tokenizer tokenizer{stream};
  EXPECT_EQ(tokenizer.peek(), Token());
  EXPECT_TRUE(tokenizer.end());
}

TEST(TokenizerTest, SkippingSpaces)
{
  std::stringstream stream{"     \n\n \n \t \v \f\r  "};
  Tokenizer tokenizer{stream};
  EXPECT_EQ(tokenizer.peek(), Token());
  EXPECT_TRUE(tokenizer.end());
}

TEST(TokenizerTest, IgnoreComments)
{
  std::stringstream stream{"// comment comment\n//Comment comment"};
  Tokenizer tokenizer{stream};
  EXPECT_EQ(tokenizer.peek(), Token());
  EXPECT_TRUE(tokenizer.end());
}

TEST(TokenizerTest, SpacesBetweenComments)
{
  std::stringstream stream{"// comment comment\n \t \v \n \f \r \r //Comment comment"};
  Tokenizer tokenizer{stream};
  EXPECT_EQ(tokenizer.peek(), Token());
  EXPECT_TRUE(tokenizer.end());
}

TEST(TokenizerTest, HandlingNumbers)
{
  std::stringstream stream{"12 -32 0.34 -0.31 2.43 -4.34"};
  std::vector<float> numbers{12,  -32, 0.34, -0.31, 2.43, -4.34};
  Tokenizer tokenizer{stream};
  for(auto num : numbers)
  {
    auto token = tokenizer.peek();
    EXPECT_FALSE(tokenizer.end());
    EXPECT_EQ(token.type, TokenType::Number);
    ASSERT_TRUE(token.numericValue.has_value());
    EXPECT_FLOAT_EQ(token.numericValue.value(), num);

    tokenizer.nextToken();
  }
  EXPECT_EQ(tokenizer.peek(), Token());
  EXPECT_TRUE(tokenizer.end());
}

TEST(TokenizerTest, Keywords)
{
  std::stringstream stream{"f32 if print fn let void"};
  std::vector<std::string> keywords{"f32", "if", "print", "fn", "let", "void"};
  Tokenizer tokenizer{stream};
  for(const auto& keyword : keywords)
  {
    auto token = tokenizer.peek();
    EXPECT_FALSE(tokenizer.end());
    EXPECT_EQ(token.type, TokenType::Keyword);
    ASSERT_TRUE(token.stringValue.has_value());
    EXPECT_EQ(token.stringValue.value(), keyword);

    tokenizer.nextToken();
  }
  EXPECT_EQ(tokenizer.peek(), Token());
  EXPECT_TRUE(tokenizer.end());
}

TEST(TokenizerTest, Identifiers)
{
  std::stringstream stream{"iden _iden iden23 iden_2324_"};
  std::vector<std::string> identifiers{"iden", "_iden", "iden23", "iden_2324_"};
  Tokenizer tokenizer{stream};
  for(const auto& identifier : identifiers)
  {
    auto token = tokenizer.peek();
    EXPECT_FALSE(tokenizer.end());
    EXPECT_EQ(token.type, TokenType::Identifier);
    ASSERT_TRUE(token.stringValue.has_value());
    EXPECT_EQ(token.stringValue.value(), identifier);

    tokenizer.nextToken();
  }
  EXPECT_EQ(tokenizer.peek(), Token());
  EXPECT_TRUE(tokenizer.end());
}

TEST(TokenizerTest, SimpleStrings)
{
  std::stringstream stream{"\"343abc_^$&#\" \"afsdf<>:PFJ4\""};
  std::vector<std::string> strings{"343abc_^$&#", "afsdf<>:PFJ4"};
  Tokenizer tokenizer{stream};
  for(const auto& str : strings)
  {
    auto token = tokenizer.peek();
    EXPECT_FALSE(tokenizer.end());
    EXPECT_EQ(token.type, TokenType::String);
    ASSERT_TRUE(token.stringValue.has_value());
    EXPECT_EQ(token.stringValue.value(), str);

    tokenizer.nextToken();
  }
  EXPECT_EQ(tokenizer.peek(), Token());
  EXPECT_TRUE(tokenizer.end());
}

TEST(TokenizerTest, StringsWithEscapeSequences)
{
  std::stringstream stream{"\"He said: \\\"Look!\\\"\" \"\\\'quote\\\' \\\? \\\\\" \"New line, etc. \\n \\t \\v \\a \\b \\r \\f\""};
  std::vector<std::string> strings{"He said: \"Look!\"", "\'quote\' \? \\", "New line, etc. \n \t \v \a \b \r \f"};
  Tokenizer tokenizer{stream};
  for(const auto& str : strings)
  {
    auto token = tokenizer.peek();
    EXPECT_FALSE(tokenizer.end());
    EXPECT_EQ(token.type, TokenType::String);
    ASSERT_TRUE(token.stringValue.has_value());
    EXPECT_EQ(token.stringValue.value(), str);

    tokenizer.nextToken();
  }
  EXPECT_EQ(tokenizer.peek(), Token());
  EXPECT_TRUE(tokenizer.end());
}