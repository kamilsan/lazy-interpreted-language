#include <gtest/gtest.h>
#include <sstream>
#include <functional>

#include "Parser.hpp"
#include "PrintVisitor.hpp"

void setupTest(const std::string& source, std::function<std::unique_ptr<Node>(Parser*)> function, std::unique_ptr<Node> expectedNode)
{
  std::stringstream ss{source};
  Parser parser{ss};
  const auto result = function(&parser);

  std::stringstream outputA{};
  PrintVisitor visitorA{outputA};
  result->accept(visitorA);

  std::stringstream outputB{};
  PrintVisitor visitorB{outputB};
  expectedNode->accept(visitorB);

  EXPECT_EQ(outputA.str(), outputB.str());
}

void testThrow(const std::string& source, std::function<std::unique_ptr<Node>(Parser*)> function)
{
  std::stringstream ss{source};
  Parser parser{ss};
  EXPECT_THROW(function(&parser), std::runtime_error);
}

TEST(ParserTest, SimpleTerms)
{
  setupTest("42", &Parser::parseTerm, std::make_unique<NumericLiteralNode>(42));
  setupTest("12.5", &Parser::parseTerm, std::make_unique<NumericLiteralNode>(12.5));
  setupTest("x", &Parser::parseTerm, std::make_unique<VariableNode>("x"));
}

TEST(ParserTest, FunctionCalls)
{
  const auto func = std::bind(&Parser::parseFunctionCall, std::placeholders::_1, std::optional<Token>{});
  std::list<std::unique_ptr<ExpressionNode>> arguments{};
  setupTest("f()", func, std::make_unique<FunctionCallNode>("f", std::move(arguments)));

  arguments = std::list<std::unique_ptr<ExpressionNode>>{};
  arguments.push_back(std::make_unique<VariableNode>("x"));
  setupTest("xyz(x)", func, std::make_unique<FunctionCallNode>("xyz", std::move(arguments)));

  arguments = std::list<std::unique_ptr<ExpressionNode>>{};
  arguments.push_back(std::make_unique<VariableNode>("x"));
  arguments.push_back(std::make_unique<NumericLiteralNode>(2));
  arguments.push_back(std::make_unique<VariableNode>("z"));
  setupTest("g(x, 2, z)", func, std::make_unique<FunctionCallNode>("g", std::move(arguments)));
}

TEST(ParserTest, InvalidFunctionCallsThrow)
{
  const auto func = std::bind(&Parser::parseFunctionCall, std::placeholders::_1, std::optional<Token>{});
  testThrow("f(", func);
  testThrow("f)", func);
  testThrow("f(x,)", func);
  testThrow("f(x y)", func);
  testThrow("(x y)", func);
}

TEST(ParserTest, StringExpressions)
{
  setupTest("\"test\"", &Parser::parseStringExpression, std::make_unique<StringLiteralNode>("test"));

  auto node = std::make_unique<BinaryOpNode>(std::make_unique<StringLiteralNode>("test"), 
    BinaryOperator::Addition, std::make_unique<NumericLiteralNode>(2));
  setupTest("\"test\" : 2", &Parser::parseStringExpression, std::move(node));

  node = std::make_unique<BinaryOpNode>(std::make_unique<StringLiteralNode>("test"), 
    BinaryOperator::Addition, std::make_unique<StringLiteralNode>("a"));
  setupTest("\"test\" : \"a\"", &Parser::parseStringExpression, std::move(node));

  node = std::make_unique<BinaryOpNode>(
    std::make_unique<BinaryOpNode>(
      std::make_unique<StringLiteralNode>("test"), 
      BinaryOperator::Addition, std::make_unique<NumericLiteralNode>(2)),
    BinaryOperator::Addition,
    std::make_unique<StringLiteralNode>("a")
  );
  setupTest("\"test\" : 2 : \"a\"", &Parser::parseStringExpression, std::move(node));
}

TEST(ParserTest, SpecialFunctionCalls)
{
  const auto func = std::bind(&Parser::parseFunctionCall, std::placeholders::_1, std::optional<Token>{});
  std::list<std::unique_ptr<ExpressionNode>> arguments{};
  arguments.push_back(std::make_unique<StringLiteralNode>("test"));
  setupTest("print(\"test\")", func, std::make_unique<FunctionCallNode>("print", std::move(arguments)));

  arguments = std::list<std::unique_ptr<ExpressionNode>>{};
  arguments.push_back(std::make_unique<NumericLiteralNode>(1));
  arguments.push_back(std::make_unique<NumericLiteralNode>(2));
  arguments.push_back(std::make_unique<VariableNode>("z"));
  setupTest("if(1, 2, z)", func, std::make_unique<FunctionCallNode>("if", std::move(arguments)));
}

TEST(ParserTest, Unary)
{
  auto node = std::make_unique<UnaryNode>(UnaryOperator::Minus, std::make_unique<NumericLiteralNode>(2));
  setupTest("-2", &Parser::parseUnary, std::move(node));

  node = std::make_unique<UnaryNode>(UnaryOperator::Minus, std::make_unique<VariableNode>("x"));
  setupTest("-x", &Parser::parseUnary, std::move(node));

  node = std::make_unique<UnaryNode>(UnaryOperator::BinaryNegation, std::make_unique<VariableNode>("x"));
  setupTest("~x", &Parser::parseUnary, std::move(node));
}

TEST(ParserTest, InvalidUnaryThrows)
{
  testThrow("+x", &Parser::parseUnary);
  testThrow("--4", &Parser::parseUnary);
}

TEST(ParserTest, Factor)
{
  auto node = std::make_unique<BinaryOpNode>(
      std::make_unique<NumericLiteralNode>(2), 
      BinaryOperator::Multiplication, 
      std::make_unique<NumericLiteralNode>(3));
  setupTest("2 * 3", &Parser::parseFactor, std::move(node));

  node = std::make_unique<BinaryOpNode>(
      std::make_unique<NumericLiteralNode>(5), 
      BinaryOperator::Division, 
      std::make_unique<NumericLiteralNode>(3));
  setupTest("5 / 3", &Parser::parseFactor, std::move(node));

  node = std::make_unique<BinaryOpNode>(
      std::make_unique<UnaryNode>(UnaryOperator::Minus, std::make_unique<VariableNode>("x")), 
      BinaryOperator::Multiplication, 
      std::make_unique<NumericLiteralNode>(2));
  setupTest("-x * 2", &Parser::parseFactor, std::move(node));

  node = std::make_unique<BinaryOpNode>(
      std::make_unique<BinaryOpNode>(std::make_unique<VariableNode>("y"), BinaryOperator::Division, std::make_unique<VariableNode>("x")), 
      BinaryOperator::Multiplication, 
      std::make_unique<NumericLiteralNode>(2));
  setupTest("y / x * 2", &Parser::parseFactor, std::move(node));
}

TEST(ParserTest, AddExpr)
{
  auto node = std::make_unique<BinaryOpNode>(
      std::make_unique<NumericLiteralNode>(2), 
      BinaryOperator::Addition, 
      std::make_unique<NumericLiteralNode>(3));
  setupTest("2 + 3", &Parser::parseAddExpression, std::move(node));

  node = std::make_unique<BinaryOpNode>(
      std::make_unique<NumericLiteralNode>(5), 
      BinaryOperator::Subtraction, 
      std::make_unique<NumericLiteralNode>(3));
  setupTest("5 - 3", &Parser::parseAddExpression, std::move(node));

  node = std::make_unique<BinaryOpNode>(
      std::make_unique<VariableNode>("x"), 
      BinaryOperator::Modulo, 
      std::make_unique<NumericLiteralNode>(2));
  setupTest("x % 2", &Parser::parseAddExpression, std::move(node));
}

TEST(ParserTest, ProperOperationOrder)
{
  auto node = std::make_unique<BinaryOpNode>(
      std::make_unique<NumericLiteralNode>(2), 
      BinaryOperator::Addition,
      std::make_unique<BinaryOpNode>(
        std::make_unique<NumericLiteralNode>(3),
        BinaryOperator::Multiplication,
        std::make_unique<NumericLiteralNode>(4)));
  setupTest("2 + 3 * 4", &Parser::parseAddExpression, std::move(node));

  node = std::make_unique<BinaryOpNode>(
      std::make_unique<BinaryOpNode>(
        std::make_unique<NumericLiteralNode>(2),
        BinaryOperator::Addition,
        std::make_unique<NumericLiteralNode>(3)), 
      BinaryOperator::Multiplication,
      std::make_unique<NumericLiteralNode>(4));
  setupTest("(2 + 3) * 4", &Parser::parseAddExpression, std::move(node));

  node = std::make_unique<BinaryOpNode>(
      std::make_unique<BinaryOpNode>(
        std::make_unique<NumericLiteralNode>(3),
        BinaryOperator::Subtraction,
        std::make_unique<NumericLiteralNode>(2)), 
      BinaryOperator::Subtraction,
      std::make_unique<NumericLiteralNode>(1));
  setupTest("3 - 2 - 1", &Parser::parseAddExpression, std::move(node));
}

TEST(ParserTest, BinaryExpr)
{
  auto node = std::make_unique<BinaryOpNode>(
      std::make_unique<NumericLiteralNode>(2), 
      BinaryOperator::BinaryAnd, 
      std::make_unique<NumericLiteralNode>(3));
  setupTest("2 & 3", &Parser::parseArithmeticExpression, std::move(node));

  node = std::make_unique<BinaryOpNode>(
      std::make_unique<NumericLiteralNode>(5), 
      BinaryOperator::BinaryOr, 
      std::make_unique<NumericLiteralNode>(3));
  setupTest("5 | 3", &Parser::parseArithmeticExpression, std::move(node));

  node = std::make_unique<BinaryOpNode>(
      std::make_unique<VariableNode>("x"), 
      BinaryOperator::BinaryXor, 
      std::make_unique<NumericLiteralNode>(2));
  setupTest("x ^ 2", &Parser::parseArithmeticExpression, std::move(node));

  node = std::make_unique<BinaryOpNode>(
      std::make_unique<NumericLiteralNode>(1), 
      BinaryOperator::ShiftLeft, 
      std::make_unique<NumericLiteralNode>(2));
  setupTest("1 << 2", &Parser::parseArithmeticExpression, std::move(node));

  node = std::make_unique<BinaryOpNode>(
      std::make_unique<NumericLiteralNode>(1), 
      BinaryOperator::ShiftRight, 
      std::make_unique<NumericLiteralNode>(2));
  setupTest("1 >> 2", &Parser::parseArithmeticExpression, std::move(node));
}

TEST(ParserTest, ArithmeticExpr)
{
  auto node = 
    std::make_unique<BinaryOpNode>(
      std::make_unique<BinaryOpNode>(
        std::make_unique<NumericLiteralNode>(2),
        BinaryOperator::Addition,
        std::make_unique<BinaryOpNode>(
          std::make_unique<NumericLiteralNode>(3),
          BinaryOperator::Multiplication,
          std::make_unique<NumericLiteralNode>(4))),
      BinaryOperator::BinaryAnd,
      std::make_unique<NumericLiteralNode>(3));
  setupTest("2 + 3 * 4 & 3", &Parser::parseArithmeticExpression, std::move(node));

  node = 
    std::make_unique<BinaryOpNode>(  
      std::make_unique<BinaryOpNode>(
        std::make_unique<BinaryOpNode>(
          std::make_unique<NumericLiteralNode>(2),
          BinaryOperator::Addition,
          std::make_unique<NumericLiteralNode>(3)), 
        BinaryOperator::Multiplication,
        std::make_unique<NumericLiteralNode>(4)),
    BinaryOperator::ShiftLeft,
    std::make_unique<NumericLiteralNode>(2));
  setupTest("(2 + 3) * 4 << 2", &Parser::parseArithmeticExpression, std::move(node));
}

TEST(ParserTest, InvalidArithmeticExprThrows)
{
  testThrow("+x + 3", &Parser::parseArithmeticExpression);
  testThrow("42++", &Parser::parseArithmeticExpression);
  testThrow("10 +", &Parser::parseArithmeticExpression);
  testThrow("10 * (23 + 3", &Parser::parseArithmeticExpression);
}

TEST(ParserTest, SimpleComparisonExpr)
{
  auto node = std::make_unique<BinaryOpNode>(
      std::make_unique<NumericLiteralNode>(2), 
      BinaryOperator::Equal, 
      std::make_unique<NumericLiteralNode>(3));
  setupTest("2 == 3", &Parser::parseComparisonExpression, std::move(node));

  node = std::make_unique<BinaryOpNode>(
      std::make_unique<NumericLiteralNode>(5), 
      BinaryOperator::NotEqual, 
      std::make_unique<NumericLiteralNode>(3));
  setupTest("5 != 3", &Parser::parseComparisonExpression, std::move(node));

  node = std::make_unique<BinaryOpNode>(
      std::make_unique<VariableNode>("x"), 
      BinaryOperator::Greater, 
      std::make_unique<NumericLiteralNode>(2));
  setupTest("x > 2", &Parser::parseComparisonExpression, std::move(node));

  node = std::make_unique<BinaryOpNode>(
      std::make_unique<NumericLiteralNode>(1), 
      BinaryOperator::Less, 
      std::make_unique<NumericLiteralNode>(2));
  setupTest("1 < 2", &Parser::parseComparisonExpression, std::move(node));

  node = std::make_unique<BinaryOpNode>(
      std::make_unique<NumericLiteralNode>(1), 
      BinaryOperator::GreaterEq, 
      std::make_unique<NumericLiteralNode>(2));
  setupTest("1 >= 2", &Parser::parseComparisonExpression, std::move(node));

  node = std::make_unique<BinaryOpNode>(
      std::make_unique<NumericLiteralNode>(1), 
      BinaryOperator::LessEq, 
      std::make_unique<NumericLiteralNode>(1));
  setupTest("1 <= 1", &Parser::parseComparisonExpression, std::move(node));
}

TEST(ParserTest, ComplexComparisonExpr)
{
  auto node = 
    std::make_unique<BinaryOpNode>(
      std::make_unique<BinaryOpNode>(
        std::make_unique<NumericLiteralNode>(2),
        BinaryOperator::Addition,
        std::make_unique<NumericLiteralNode>(3)),
      BinaryOperator::Greater,
      std::make_unique<NumericLiteralNode>(5));
  setupTest("2 + 3 > 5", &Parser::parseComparisonExpression, std::move(node));

  node = 
    std::make_unique<BinaryOpNode>(  
      std::make_unique<BinaryOpNode>(
        std::make_unique<BinaryOpNode>(
          std::make_unique<NumericLiteralNode>(2),
          BinaryOperator::Addition,
          std::make_unique<NumericLiteralNode>(3)), 
        BinaryOperator::Multiplication,
        std::make_unique<NumericLiteralNode>(4)),
    BinaryOperator::GreaterEq,
     std::make_unique<BinaryOpNode>(
      std::make_unique<NumericLiteralNode>(2),
      BinaryOperator::BinaryXor,
      std::make_unique<NumericLiteralNode>(4))); 
  setupTest("(2 + 3) * 4 >= 2 ^ 4", &Parser::parseComparisonExpression, std::move(node));
}

TEST(ParserTest, UnaryLogical)
{
  std::unique_ptr<ExpressionNode> node = 
    std::make_unique<BinaryOpNode>(
      std::make_unique<NumericLiteralNode>(2),
      BinaryOperator::Greater,
      std::make_unique<NumericLiteralNode>(3));
  setupTest("(2 > 3)", &Parser::parseUnaryLogical, std::move(node));

  node = 
    std::make_unique<UnaryNode>(
      UnaryOperator::LogicalNot,
      std::make_unique<BinaryOpNode>(
        std::make_unique<NumericLiteralNode>(2),
        BinaryOperator::Greater,
        std::make_unique<NumericLiteralNode>(3)));
  setupTest("!(2 > 3)", &Parser::parseUnaryLogical, std::move(node));

  node = 
    std::make_unique<UnaryNode>(
      UnaryOperator::LogicalNot,
      std::make_unique<BinaryOpNode>(    
        std::make_unique<NumericLiteralNode>(1),
        BinaryOperator::Less,
        std::make_unique<NumericLiteralNode>(3)));
  setupTest("!1 < 3", &Parser::parseUnaryLogical, std::move(node));
}

TEST(ParserTest, LogicalExpression)
{
  auto node = 
    std::make_unique<BinaryOpNode>(
      std::make_unique<BinaryOpNode>(
        std::make_unique<NumericLiteralNode>(2),
        BinaryOperator::GreaterEq,
        std::make_unique<NumericLiteralNode>(5)),
      BinaryOperator::LogicalAnd,
      std::make_unique<BinaryOpNode>(
        std::make_unique<NumericLiteralNode>(2),
        BinaryOperator::Equal,
        std::make_unique<NumericLiteralNode>(2)));
  setupTest("2 >= 5 && 2 == 2", &Parser::parseLogicalExpression, std::move(node));

  node = 
    std::make_unique<BinaryOpNode>(
      std::make_unique<BinaryOpNode>(
        std::make_unique<BinaryOpNode>(
          std::make_unique<NumericLiteralNode>(2),
          BinaryOperator::LessEq,
          std::make_unique<NumericLiteralNode>(5)),
        BinaryOperator::LogicalOr,
        std::make_unique<BinaryOpNode>(
          std::make_unique<NumericLiteralNode>(2),
          BinaryOperator::Equal,
          std::make_unique<NumericLiteralNode>(2))),
      BinaryOperator::LogicalAnd,
      std::make_unique<BinaryOpNode>(
        std::make_unique<NumericLiteralNode>(4),
        BinaryOperator::Greater,
        std::make_unique<NumericLiteralNode>(2)));
  setupTest("2 <= 5 || 2 == 2 && 4 > 2", &Parser::parseLogicalExpression, std::move(node));
}

TEST(ParserTest, VariableDeclaration)
{
  auto node = std::make_unique<VariableDeclarationNode>(
    "x",
    TypeName::F32,
    std::make_unique<NumericLiteralNode>(1)
  );
  setupTest("let x: f32 = 1;", &Parser::parseVariableDeclaration, std::move(node));

  node = std::make_unique<VariableDeclarationNode>(
    "xyz",
    TypeName::F32,
    std::make_unique<BinaryOpNode>(
      std::make_unique<NumericLiteralNode>(2), 
      BinaryOperator::Addition,
      std::make_unique<BinaryOpNode>(
        std::make_unique<NumericLiteralNode>(3),
        BinaryOperator::Multiplication,
        std::make_unique<NumericLiteralNode>(4))));
  setupTest("let xyz: f32 = 2+3*4;", &Parser::parseVariableDeclaration, std::move(node));
}

TEST(ParserTest, InvalidVariableDeclThrows)
{
  testThrow("xyz: f32 = 2;", &Parser::parseVariableDeclaration);
  testThrow("let : f32 = 4;", &Parser::parseVariableDeclaration);
  testThrow("let x f32 = 6+4;", &Parser::parseVariableDeclaration);
  testThrow("let y: = 5;", &Parser::parseVariableDeclaration);
  testThrow("let h: f3fsg2 = 5;", &Parser::parseVariableDeclaration);
  testThrow("let d: f32  5;", &Parser::parseVariableDeclaration);
  testThrow("let k: f32 = ;", &Parser::parseVariableDeclaration);
  testThrow("let l: f32 = 6", &Parser::parseVariableDeclaration);
  testThrow("let let: f32 = 6;", &Parser::parseVariableDeclaration);
  testThrow("let if: f32 = 6;", &Parser::parseVariableDeclaration);
  testThrow("let print: f32 = 6;", &Parser::parseVariableDeclaration);
  testThrow("let f32: f32 = 6;", &Parser::parseVariableDeclaration);
  testThrow("let void: f32 = 6;", &Parser::parseVariableDeclaration);
  testThrow("let fn: f32 = 6;", &Parser::parseVariableDeclaration);
  testThrow("let function: f32 = 6;", &Parser::parseVariableDeclaration);
}

TEST(ParserTest, SimpleAssignment)
{
  const auto func = std::bind(&Parser::parseAssignment, std::placeholders::_1, std::optional<Token>{});
  const auto test = [func](const std::string& stringOp, AssignmentOperator op) {
    auto node = std::make_unique<AssignmentNode>(
      "x",
      op,
      std::make_unique<NumericLiteralNode>(3)
    );
    setupTest("x" + stringOp + "3;", func, std::move(node));
  };

  test("=", AssignmentOperator::Assign);
  test("+=", AssignmentOperator::PlusEq);
  test("-=", AssignmentOperator::MinusEq);
  test("*=", AssignmentOperator::MulEq);
  test("/=", AssignmentOperator::DivEq);
  test("&=", AssignmentOperator::AndEq);
  test("|=", AssignmentOperator::OrEq);
  test("^=", AssignmentOperator::XorEq);
  test("<<=", AssignmentOperator::ShiftLeftEq);
  test(">>=", AssignmentOperator::ShiftRightEq);
}

TEST(ParserTest, InvalidAssignmentThrows)
{
  const auto func = std::bind(&Parser::parseAssignment, std::placeholders::_1, std::optional<Token>{});
  testThrow("= 3;", func);
  testThrow("y = ;", func);
  testThrow("y = 3", func);
}

TEST(ParserTest, ReturnStatement)
{
  auto node = std::make_unique<ReturnNode>(std::make_unique<NumericLiteralNode>(12));
  setupTest("ret 12;", &Parser::parseReturnStatement, std::move(node));
}

TEST(ParserTest, InvalidReturnThrows)
{
  testThrow("3;", &Parser::parseReturnStatement);
  testThrow("ret ;", &Parser::parseReturnStatement);
  testThrow("ret 3", &Parser::parseReturnStatement);
}

TEST(ParserTest, Block)
{
  auto block = std::make_unique<BlockNode>();
  block->addStatement(std::make_unique<ReturnNode>(std::make_unique<NumericLiteralNode>(12)));
  setupTest("{ ret 12; }", &Parser::parseBlock, std::move(block));

  block = std::make_unique<BlockNode>();
  block->addStatement(
    std::make_unique<VariableDeclarationNode>(
      "x", 
      TypeName::F32,
      std::make_unique<NumericLiteralNode>(42)));
  setupTest("{ let x:f32=42; }", &Parser::parseBlock, std::move(block));

  block = std::make_unique<BlockNode>();
  block->addStatement(
    std::make_unique<AssignmentNode>(
      "x", 
      AssignmentOperator::Assign,
      std::make_unique<NumericLiteralNode>(7)));
  setupTest("{ x=7; }", &Parser::parseBlock, std::move(block));

  block = std::make_unique<BlockNode>();
  auto args = std::list<std::unique_ptr<ExpressionNode>>{};
  args.push_back(std::make_unique<StringLiteralNode>("test"));
  block->addStatement(
    std::make_unique<FunctionCallStatementNode>(std::make_unique<FunctionCallNode>(
      "print",
      std::move(args)
    )));
  setupTest("{ print(\"test\"); }", &Parser::parseBlock, std::move(block));

  block = std::make_unique<BlockNode>();
  block->addStatement(
    std::make_unique<VariableDeclarationNode>(
      "x", 
      TypeName::F32,
      std::make_unique<NumericLiteralNode>(42)));
  block->addStatement(
    std::make_unique<AssignmentNode>(
      "x", 
      AssignmentOperator::Assign,
      std::make_unique<NumericLiteralNode>(7)));
  block->addStatement(std::make_unique<ReturnNode>(std::make_unique<NumericLiteralNode>(12)));
  setupTest("{ let x: f32 = 42; x=7; ret 12; }", &Parser::parseBlock, std::move(block));
}

TEST(ParserTest, InvalidBlockThrows)
{
  testThrow(" ret 3; }", &Parser::parseBlock);
  testThrow("{ f(); ", &Parser::parseBlock);
  testThrow("{ f(); ggdfh }", &Parser::parseBlock);
  testThrow("{ 4 }", &Parser::parseBlock);
  testThrow("{ 4+3; }", &Parser::parseBlock);
}

TEST(ParserTest, FunctionDeclaration)
{
  std::list<std::pair<std::string, TypeName>> args{};
  auto block = std::make_unique<BlockNode>();
  auto node = std::make_unique<FunctionDeclarationNode>(
    "f",
    TypeName::F32,
    std::move(args),
    std::move(block)
  );
  setupTest("fn f(): f32 {}", &Parser::parseFunctionDeclaration, std::move(node));

  args = std::list<std::pair<std::string, TypeName>>{};
  args.push_back(std::make_pair<std::string, TypeName>("x", TypeName::F32));
  block = std::make_unique<BlockNode>();
  node = std::make_unique<FunctionDeclarationNode>(
    "g",
    TypeName::F32,
    std::move(args),
    std::move(block)
  );
  setupTest("fn g(x: f32): f32 {}", &Parser::parseFunctionDeclaration, std::move(node));

  args = std::list<std::pair<std::string, TypeName>>{};
  args.push_back(std::make_pair<std::string, TypeName>("x", TypeName::F32));
  args.push_back(std::make_pair<std::string, TypeName>("y", TypeName::Function));
  block = std::make_unique<BlockNode>();
  block->addStatement(std::make_unique<ReturnNode>(std::make_unique<NumericLiteralNode>(12)));
  node = std::make_unique<FunctionDeclarationNode>(
    "g",
    TypeName::Void,
    std::move(args),
    std::move(block)
  );
  setupTest("fn g(x: f32, y: function): void { ret 12; }", &Parser::parseFunctionDeclaration, std::move(node));
}

TEST(ParserTest, InvalidFunctionDeclarationThrows)
{
  testThrow("t(x: f32): f32 { }", &Parser::parseFunctionDeclaration);
  testThrow("fn (x:f32): f32 {}", &Parser::parseFunctionDeclaration);
  testThrow("fn t :f32): f32 {}", &Parser::parseFunctionDeclaration);
  testThrow("fn t(x f32) :f32 {}", &Parser::parseFunctionDeclaration);
  testThrow("fn f(x: f32) f32 {}", &Parser::parseFunctionDeclaration);
  testThrow("fn f(x: f32) : {}", &Parser::parseFunctionDeclaration);
  testThrow("fn f(x: f32, ): f32 {}", &Parser::parseFunctionDeclaration);
  testThrow("fn f(x: f32, y) : f32 {}", &Parser::parseFunctionDeclaration);
  testThrow("fn f(x: f32) : f32", &Parser::parseFunctionDeclaration);
  testThrow("fn f(x: f32) : f32 {", &Parser::parseFunctionDeclaration);
  testThrow("fn f(x: f32) : f32 }", &Parser::parseFunctionDeclaration);
}

TEST(ParserTest, LambdaDeclaration)
{
  std::list<std::pair<std::string, TypeName>> args{};
  auto block = std::make_unique<BlockNode>();
  auto node = std::make_unique<LambdaNode>(
    TypeName::F32,
    std::move(args),
    std::move(block)
  );
  setupTest("\\(): f32 = {}", &Parser::parseLambda, std::move(node));

  args = std::list<std::pair<std::string, TypeName>>{};
  args.push_back(std::make_pair<std::string, TypeName>("x", TypeName::F32));
  block = std::make_unique<BlockNode>();
  node = std::make_unique<LambdaNode>(
    TypeName::Void,
    std::move(args),
    std::move(block)
  );
  setupTest("\\(x: f32): void = {}", &Parser::parseLambda, std::move(node));
}

TEST(ParserTest, InvalidLambdaDeclarationThrows)
{
  testThrow("(x: f32): f32 = { }", &Parser::parseLambda);
  testThrow("\\x:f32): f32 = {}", &Parser::parseLambda);
  testThrow("\\( :f32): f32 = {}", &Parser::parseLambda);
  testThrow("\\(x f32) :f32 = {}", &Parser::parseLambda);
  testThrow("\\(x:): f32 = {}", &Parser::parseLambda);
  testThrow("\\(x: f32) f32 = {}", &Parser::parseLambda);
  testThrow("\\(x: f32):  = {}", &Parser::parseLambda);
  testThrow("\\(x: f32) : f32  {}", &Parser::parseLambda);
  testThrow("\\(x: f32) : f32 = {", &Parser::parseLambda);
  testThrow("\\f(x: f32) : f32 = {}", &Parser::parseLambda);
  testThrow("\\(x: f32) : f32 = }", &Parser::parseLambda);
}

TEST(ParserTest, CallingLambda)
{
  const auto func = std::bind(&Parser::parseLambdaCall, std::placeholders::_1, false);
  std::list<std::pair<std::string, TypeName>> args{};
  args.push_back(std::make_pair<std::string, TypeName>("x", TypeName::F32));
  auto block = std::make_unique<BlockNode>();
  auto lambda = std::make_unique<LambdaNode>(
    TypeName::Void,
    std::move(args),
    std::move(block)
  );
  std::list<std::unique_ptr<ExpressionNode>> callArgs{};
  callArgs.push_back(std::make_unique<NumericLiteralNode>(3));
  auto node = std::make_unique<LambdaCallNode>(std::move(lambda), std::move(callArgs));
  setupTest("(\\(x: f32): void = {})(3)", func, std::move(node));
}

TEST(ParserTest, LambdaInVarDecl)
{
  std::list<std::pair<std::string, TypeName>> args{};
  args.push_back(std::make_pair<std::string, TypeName>("x", TypeName::F32));
  auto block = std::make_unique<BlockNode>();
  auto lambda = std::make_unique<LambdaNode>(
    TypeName::Void,
    std::move(args),
    std::move(block)
  );

  auto node = std::make_unique<VariableDeclarationNode>(
    "x",
    TypeName::Function,
    std::move(lambda)
  );

  setupTest("let x: function = \\(x: f32): void = {};", &Parser::parseVariableDeclaration, std::move(node));
}

TEST(ParserTest, LambdaAsCallArgument)
{
  const auto func = std::bind(&Parser::parseFunctionCall, std::placeholders::_1, std::optional<Token>{});

  std::list<std::pair<std::string, TypeName>> args{};
  args.push_back(std::make_pair<std::string, TypeName>("x", TypeName::F32));
  auto block = std::make_unique<BlockNode>();
  auto lambda = std::make_unique<LambdaNode>(
    TypeName::F32,
    std::move(args),
    std::move(block)
  );

  std::list<std::unique_ptr<ExpressionNode>> callArgs{};
  callArgs.push_back(std::move(lambda));
  auto node = std::make_unique<FunctionCallNode>(
    "func",
    std::move(callArgs)
  );

  setupTest("func(\\(x:f32): f32 = {})", func, std::move(node));
}

TEST(ParserTest, AssignLambda)
{
  const auto func = std::bind(&Parser::parseAssignment, std::placeholders::_1, std::optional<Token>{});
  std::list<std::pair<std::string, TypeName>> args{};
  args.push_back(std::make_pair<std::string, TypeName>("x", TypeName::F32));
  auto block = std::make_unique<BlockNode>();
  auto lambda = std::make_unique<LambdaNode>(
    TypeName::F32,
    std::move(args),
    std::move(block)
  );

  auto node = std::make_unique<AssignmentNode>(
    "x",
    AssignmentOperator::Assign,
    std::move(lambda)
  );

  setupTest("x = \\(x: f32): f32 = {};", func, std::move(node));
}

TEST(ParserTest, InvalidLambdaAssignmentThrows)
{
  testThrow("x += \\(x: f32): f32 = {};", &Parser::parseLambda);
  testThrow("x -= \\(x: f32): f32 = {};", &Parser::parseLambda);
  testThrow("x *= \\(x: f32): f32 = {};", &Parser::parseLambda);
  testThrow("x /= \\(x: f32): f32 = {};", &Parser::parseLambda);
  testThrow("x &= \\(x: f32): f32 = {};", &Parser::parseLambda);
  testThrow("x |= \\(x: f32): f32 = {};", &Parser::parseLambda);
  testThrow("x <<= \\(x: f32): f32 = {};", &Parser::parseLambda);
  testThrow("x >>= \\(x: f32): f32 = {};", &Parser::parseLambda);
}

TEST(ParserTest, CallingFunctionResult)
{
  const auto func = std::bind(&Parser::parseFunctionCall, std::placeholders::_1, std::optional<Token>{});
  std::list<std::unique_ptr<ExpressionNode>> arguments{};
  arguments.push_back(std::make_unique<VariableNode>("x"));
  arguments.push_back(std::make_unique<NumericLiteralNode>(2));

  std::list<std::unique_ptr<ExpressionNode>> args2{};
  args2.push_back(std::make_unique<NumericLiteralNode>(10));

  auto funcCallNode = std::make_unique<FunctionCallNode>("f", std::move(arguments));
  auto node = std::make_unique<FunctionResultCallNode>(std::move(funcCallNode), std::move(args2));

  setupTest("f(x, 2)(10)", func, std::move(node));
}