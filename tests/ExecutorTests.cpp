#include <gtest/gtest.h>
#include <sstream>

#include "AST.hpp"
#include "Parser.hpp"
#include "Executor.hpp"

void testExpression(const std::string expr, double value)
{
  std::stringstream stream{expr};
  Parser parser{stream};
  auto node = parser.parseLogicalExpression();

  Executor executor{};
  node->accept(executor);
  const auto& val = executor.getValue();
  ASSERT_EQ(val->getType(), TypeName::F32);

  NumberValueAnalyser analyser{};
  val->accept(analyser);
  const auto res = analyser.getValue().value();
  EXPECT_DOUBLE_EQ(res, value);
}

void testProgram(const std::string code, const std::string& out, int status)
{
  std::stringstream stream{code};
  Parser parser{stream};
  auto program = parser.parseProgram();

  Executor executor{};
  program->accept(executor);

  EXPECT_EQ(executor.getStandardOut(), out);
  EXPECT_EQ(executor.getExitCode(), status);
}

TEST(ExecutorTest, BasicFactor)
{
  testExpression("2*3", 6);
  testExpression("6/2", 3);
}

TEST(ExecutorTest, BasicAdditiveExpr)
{
  testExpression("2+3", 5);
  testExpression("6-2", 4);
  testExpression("10%3", 1);
}

TEST(ExecutorTest, BasicUnaryExpr)
{
  testExpression("-2", -2);
  testExpression("~2", 4294967293);
}

TEST(ExecutorTest, ProperOperationOrder)
{
  testExpression("-2 + 5 * 2", 8);
  testExpression("3 - 2 - 1", 0);
}

TEST(ExecutorTest, CompoundArithExpr)
{
  testExpression("(-2 + 5) * 2 + (4 >> 1)", 8);
  testExpression("((2 | 1) + 1) / 2", 2);
}

TEST(ExecutorTest, ComparisonExpr)
{
  testExpression("2 == 2", 1);
  testExpression("2*2 >= 42", 0);
}

TEST(ExecutorTest, UnaryLogicalExpr)
{
  testExpression("!(2 == 2)", 0);
  testExpression("!(2*2 >= 42)", 1);
}

TEST(ExecutorTest, CompoundLogicalExpr)
{
  testExpression("!(2 == 2) || 3 > 2", 1);
  testExpression("!(2*2 >= 42) && 2/2 == 1", 1);
}

TEST(ExecutorTest, ExitCodeWorks)
{
  std::string source = R"SRC(
  fn main(): f32
  {
    ret 12;
  }
  )SRC";

  std::string expected = "";

  testProgram(source, expected, 12);
}


TEST(ExecutorTest, PrintWorks)
{
  std::string source = R"SRC(
  fn main(): f32
  {
    print("test!");
    ret 0;
  }
  )SRC";

  std::string expected = "test!\n";

  testProgram(source, expected, 0);
}


TEST(ExecutorTest, PrintWithConcatWorks)
{
  std::string source = R"SRC(
  fn main(): f32
  {
    print("test " : 1 : " other");
    ret 0;
  }
  )SRC";

  std::string expected = "test 1.000000 other\n";

  testProgram(source, expected, 0);
}

TEST(ExecutorTest, VariableWorks)
{
  std::string source = R"SRC(
  let x: f32 = 1;

  fn main(): f32
  {
    print("" : x);
    ret 0;
  }
  )SRC";

  std::string expected = "1.000000\n";

  testProgram(source, expected, 0);
}


TEST(ExecutorTest, VariableShadowingWorks)
{
  std::string source = R"SRC(
  let x: f32 = 1;

  fn main(): f32
  {
    let x: f32 = 2;
    print("" : x);
    ret 0;
  }
  )SRC";

  std::string expected = "2.000000\n";

  testProgram(source, expected, 0);
}

TEST(ExecutorTest, FunctionCallWorks)
{
  std::string source = R"SRC(
  fn test(x: f32, y: f32): f32
  {
    ret x + y + 1;
  }

  fn main(): f32
  {
    print("" : test(1, 2));
    ret 0;
  }
  )SRC";

  std::string expected = "4.000000\n";

  testProgram(source, expected, 0);
}


TEST(ExecutorTest, VoidFunctionCallWorks)
{
  std::string source = R"SRC(
  fn test(x: f32): void
  {
    print("test " : x);
  }

  fn main(): f32
  {
    test(4);
    ret 0;
  }
  )SRC";

  std::string expected = "test 4.000000\n";

  testProgram(source, expected, 0);
}

TEST(ExecutorTest, RecursionWorks)
{
  std::string source = R"SRC(
  fn factorial(n: f32): f32
  {
    ret if(n == 0, 1, n * factorial(n - 1));
  }

  fn main(): f32
  {
    print("" : factorial(4));
    ret 0;
  }
  )SRC";

  std::string expected = "24.000000\n";

  testProgram(source, expected, 0);
}

TEST(ExecutorTest, IfWorks)
{
  std::string source = R"SRC(
  fn main(): f32
  {
    let t1: f32 = if(1==1, 1, 0);
    let t2: f32 = if(1!=1, 1, 0);
    print("" : t1 : " " : t2);
    ret 0;
  }
  )SRC";

  std::string expected = "1.000000 0.000000\n";

  testProgram(source, expected, 0);
}

TEST(ExecutorTest, LambdaCall)
{
  std::string source = R"SRC(
  fn main(): f32
  {
    let x: f32 = (\(y: f32, z: f32): f32 = { ret y + z; })(1, 2);
    print("" : x);
    ret 0;
  }
  )SRC";

  std::string expected = "3.000000\n";

  testProgram(source, expected, 0);
}

TEST(ExecutorTest, LambdaCallFromVar)
{
  std::string source = R"SRC(
  fn main(): f32
  {
    let f: function = \(y: f32, z: f32): f32 = { ret y + z; };
    print("" : f(2, 2));
    ret 0;
  }
  )SRC";

  std::string expected = "4.000000\n";

  testProgram(source, expected, 0);
}

TEST(ExecutorTest, LambdaContextCapture)
{
  std::string source = R"SRC(
  fn main(): f32
  {
    let m: f32 = 1;
    let f: function = \(y: f32, z: f32): f32 = { ret y + z + m; };
    m = 2;
    print("" : f(2, 2));
    ret 0;
  }
  )SRC";

  std::string expected = "5.000000\n";

  testProgram(source, expected, 0);
}

TEST(ExecutorTest, Assignment)
{
  std::string source = R"SRC(
  fn main(): f32
  {
    let m: f32 = 1;
    m = 2;
    print("" : m);
    m += 2;
    print("" : m);
    m <<= 1;
    print("" : m);
    ret 0;
  }
  )SRC";

  std::string expected = "2.000000\n4.000000\n8.000000\n";

  testProgram(source, expected, 0);
}

TEST(ExecutorTest, AssignFunctionToVariable)
{
  std::string source = R"SRC(
  fn test(): void
  {
    print("test");
  }

  fn main(): f32
  {
    let f: function = test;
    f();
    ret 0;
  }
  )SRC";

  std::string expected = "test\n";

  testProgram(source, expected, 0);
}

TEST(ExecutorTest, LazyEvaluation)
{
  std::string source = R"SRC(
  fn hang(): f32
  {
    ret hang();
  }

  fn main(): f32
  {
    let f: f32 = hang();
    ret 0;
  }
  )SRC";

  std::string expected = "";

  testProgram(source, expected, 0);
}