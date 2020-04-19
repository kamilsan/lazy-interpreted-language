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
  std::stringstream stream{"12 14.5 0.34 0.31 0 2.43"};
  std::vector<float> numbers{12, 14.5, 0.34, 0.31, 0, 2.43};

  Tokenizer tokenizer{stream};
  for(auto num : numbers)
  {
    auto token = tokenizer.peek();
    EXPECT_FALSE(tokenizer.end());
    EXPECT_EQ(token.type, TokenType::Number);
    ASSERT_TRUE(token.numericValue.has_value());
    EXPECT_FLOAT_EQ(token.numericValue.value(), num);

    EXPECT_FALSE(Token::isArithmeticOperator(token));
    EXPECT_FALSE(Token::isAssigmentOperator(token));
    EXPECT_FALSE(Token::isBinaryOperator(token));
    EXPECT_FALSE(Token::isComparisonOperator(token));
    EXPECT_FALSE(Token::isLogicalOperator(token));

    tokenizer.nextToken();
  }
  EXPECT_EQ(tokenizer.peek(), Token());
  EXPECT_TRUE(tokenizer.end());
}

TEST(TokenizerTest, Keywords)
{
  std::stringstream stream{"f32 if print fn let void ret"};
  std::vector<std::string> keywords{"f32", "if", "print", "fn", "let", "void", "ret"};

  Tokenizer tokenizer{stream};
  for(const auto& keyword : keywords)
  {
    auto token = tokenizer.peek();
    EXPECT_FALSE(tokenizer.end());
    EXPECT_EQ(token.type, TokenType::Keyword);
    ASSERT_TRUE(token.stringValue.has_value());
    EXPECT_EQ(token.stringValue.value(), keyword);

    EXPECT_FALSE(Token::isArithmeticOperator(token));
    EXPECT_FALSE(Token::isAssigmentOperator(token));
    EXPECT_FALSE(Token::isBinaryOperator(token));
    EXPECT_FALSE(Token::isComparisonOperator(token));
    EXPECT_FALSE(Token::isLogicalOperator(token));

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

    EXPECT_FALSE(Token::isArithmeticOperator(token));
    EXPECT_FALSE(Token::isAssigmentOperator(token));
    EXPECT_FALSE(Token::isBinaryOperator(token));
    EXPECT_FALSE(Token::isComparisonOperator(token));
    EXPECT_FALSE(Token::isLogicalOperator(token));

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

    EXPECT_FALSE(Token::isArithmeticOperator(token));
    EXPECT_FALSE(Token::isAssigmentOperator(token));
    EXPECT_FALSE(Token::isBinaryOperator(token));
    EXPECT_FALSE(Token::isComparisonOperator(token));
    EXPECT_FALSE(Token::isLogicalOperator(token));

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

    EXPECT_FALSE(Token::isArithmeticOperator(token));
    EXPECT_FALSE(Token::isAssigmentOperator(token));
    EXPECT_FALSE(Token::isBinaryOperator(token));
    EXPECT_FALSE(Token::isComparisonOperator(token));
    EXPECT_FALSE(Token::isLogicalOperator(token));

    tokenizer.nextToken();
  }
  EXPECT_EQ(tokenizer.peek(), Token());
  EXPECT_TRUE(tokenizer.end());
}

TEST(TokenizerTest, ArithmeticOperators)
{
  std::stringstream stream{"+ - * / %"};
  std::vector<std::pair<std::string, TokenType>> tokensInfo{
    std::make_pair("+", TokenType::Plus), 
    std::make_pair("-", TokenType::Minus), 
    std::make_pair("*", TokenType::Mul), 
    std::make_pair("/", TokenType::Div), 
    std::make_pair("%", TokenType::Modulo)
    };
  
  Tokenizer tokenizer{stream};
  for(const auto& tokenInfo : tokensInfo)
  {
    auto token = tokenizer.peek();
    EXPECT_FALSE(tokenizer.end());
    EXPECT_EQ(token.type, tokenInfo.second);
    ASSERT_TRUE(token.stringValue.has_value());
    EXPECT_EQ(token.stringValue.value(), tokenInfo.first);

    EXPECT_TRUE(Token::isArithmeticOperator(token));
    EXPECT_FALSE(Token::isAssigmentOperator(token));
    EXPECT_FALSE(Token::isBinaryOperator(token));
    EXPECT_FALSE(Token::isComparisonOperator(token));
    EXPECT_FALSE(Token::isLogicalOperator(token));

    tokenizer.nextToken();
  }
  EXPECT_EQ(tokenizer.peek(), Token());
  EXPECT_TRUE(tokenizer.end());
}

TEST(TokenizerTest, AssignmentOperators)
{
  std::stringstream stream{"= += -= *= /= &= |= ^= <<= >>="};
  std::vector<std::pair<std::string, TokenType>> tokensInfo{
    std::make_pair("=", TokenType::Assign), 
    std::make_pair("+=", TokenType::PlusEq), 
    std::make_pair("-=", TokenType::MinusEq), 
    std::make_pair("*=", TokenType::MulEq), 
    std::make_pair("/=", TokenType::DivEq), 
    std::make_pair("&=", TokenType::AndEq), 
    std::make_pair("|=", TokenType::OrEq), 
    std::make_pair("^=", TokenType::XorEq), 
    std::make_pair("<<=", TokenType::ShiftLeftEq), 
    std::make_pair(">>=", TokenType::ShiftRightEq)
    };

  Tokenizer tokenizer{stream};
  for(const auto& tokenInfo : tokensInfo)
  {
    auto token = tokenizer.peek();
    EXPECT_FALSE(tokenizer.end());
    EXPECT_EQ(token.type, tokenInfo.second);
    ASSERT_TRUE(token.stringValue.has_value());
    EXPECT_EQ(token.stringValue.value(), tokenInfo.first);

    EXPECT_FALSE(Token::isArithmeticOperator(token));
    EXPECT_TRUE(Token::isAssigmentOperator(token));
    EXPECT_FALSE(Token::isBinaryOperator(token));
    EXPECT_FALSE(Token::isComparisonOperator(token));
    EXPECT_FALSE(Token::isLogicalOperator(token));

    tokenizer.nextToken();
  }
  EXPECT_EQ(tokenizer.peek(), Token());
  EXPECT_TRUE(tokenizer.end());
}

TEST(TokenizerTest, BinaryOperators)
{
  std::stringstream stream{"~ & | ^ >> <<"};
  std::vector<std::pair<std::string, TokenType>> tokensInfo{
    std::make_pair("~", TokenType::BinaryNot), 
    std::make_pair("&", TokenType::BinaryAnd), 
    std::make_pair("|", TokenType::BinaryOr), 
    std::make_pair("^", TokenType::BinaryXor), 
    std::make_pair(">>", TokenType::ShiftRight),
    std::make_pair("<<", TokenType::ShiftLeft)
    };

  Tokenizer tokenizer{stream};
  for(const auto& tokenInfo : tokensInfo)
  {
    auto token = tokenizer.peek();
    EXPECT_FALSE(tokenizer.end());
    EXPECT_EQ(token.type, tokenInfo.second);
    ASSERT_TRUE(token.stringValue.has_value());
    EXPECT_EQ(token.stringValue.value(), tokenInfo.first);

    EXPECT_FALSE(Token::isArithmeticOperator(token));
    EXPECT_FALSE(Token::isAssigmentOperator(token));
    EXPECT_TRUE(Token::isBinaryOperator(token));
    EXPECT_FALSE(Token::isComparisonOperator(token));
    EXPECT_FALSE(Token::isLogicalOperator(token));

    tokenizer.nextToken();
  }
  EXPECT_EQ(tokenizer.peek(), Token());
  EXPECT_TRUE(tokenizer.end());
}

TEST(TokenizerTest, ComparisonOperators)
{
  std::stringstream stream{"== != > >= < <="};
  std::vector<std::pair<std::string, TokenType>> tokensInfo{
    std::make_pair("==", TokenType::Equal), 
    std::make_pair("!=", TokenType::NotEqual), 
    std::make_pair(">", TokenType::Greater), 
    std::make_pair(">=", TokenType::GreaterOrEqual), 
    std::make_pair("<", TokenType::Less),
    std::make_pair("<=", TokenType::LessOrEqual)
    };

  Tokenizer tokenizer{stream};
  for(const auto& tokenInfo : tokensInfo)
  {
    auto token = tokenizer.peek();
    EXPECT_FALSE(tokenizer.end());
    EXPECT_EQ(token.type, tokenInfo.second);
    ASSERT_TRUE(token.stringValue.has_value());
    EXPECT_EQ(token.stringValue.value(), tokenInfo.first);

    EXPECT_FALSE(Token::isArithmeticOperator(token));
    EXPECT_FALSE(Token::isAssigmentOperator(token));
    EXPECT_FALSE(Token::isBinaryOperator(token));
    EXPECT_TRUE(Token::isComparisonOperator(token));
    EXPECT_FALSE(Token::isLogicalOperator(token));

    tokenizer.nextToken();
  }
  EXPECT_EQ(tokenizer.peek(), Token());
  EXPECT_TRUE(tokenizer.end());
}

TEST(TokenizerTest, LogicalOperators)
{
  std::stringstream stream{"! && ||"};
  std::vector<std::pair<std::string, TokenType>> tokensInfo{
    std::make_pair("!", TokenType::LogicalNot), 
    std::make_pair("&&", TokenType::LogicalAnd), 
    std::make_pair("||", TokenType::LogicalOr)
    };

  Tokenizer tokenizer{stream};
  for(const auto& tokenInfo : tokensInfo)
  {
    auto token = tokenizer.peek();
    EXPECT_FALSE(tokenizer.end());
    EXPECT_EQ(token.type, tokenInfo.second);
    ASSERT_TRUE(token.stringValue.has_value());
    EXPECT_EQ(token.stringValue.value(), tokenInfo.first);

    EXPECT_FALSE(Token::isArithmeticOperator(token));
    EXPECT_FALSE(Token::isAssigmentOperator(token));
    EXPECT_FALSE(Token::isBinaryOperator(token));
    EXPECT_FALSE(Token::isComparisonOperator(token));
    EXPECT_TRUE(Token::isLogicalOperator(token));

    tokenizer.nextToken();
  }
  EXPECT_EQ(tokenizer.peek(), Token());
  EXPECT_TRUE(tokenizer.end());
}
