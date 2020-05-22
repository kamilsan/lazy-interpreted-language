#include <gtest/gtest.h>
#include <sstream>

#include "AST.hpp"
#include "Parser.hpp"
#include "SemanticAnalyser.hpp"

void throwTest(const std::string& source)
{
  std::stringstream ss{source};
  Parser parser{ss};

  auto node = parser.parseProgram();
  SemanticAnalyser semantic{};

  EXPECT_THROW(node->accept(semantic), std::runtime_error);
}

void nothrowTest(const std::string& source)
{
  std::stringstream ss{source};
  Parser parser{ss};

  auto node = parser.parseProgram();
  SemanticAnalyser semantic{};

  EXPECT_NO_THROW(node->accept(semantic));
}


TEST(SemanticAnalyserTest, VariableRedefinitionThrows)
{
  std::string source = R"SRC(
  fn main(): f32
  {
    let x: f32 = 2;
    let x: f32 = 5;

    ret 0;
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, VariableDefinitionsWork)
{
  std::string source = R"SRC(
  fn main(): f32
  {
    let x: f32 = 2;
    let y: f32 = 5;

    ret 0;
  }
  )SRC";
  nothrowTest(source);
}

TEST(SemanticAnalyserTest, FunctionRedefinitionThrows)
{
  std::string source = R"SRC(
  fn f(x: f32): f32 { ret 1; }
  fn f(x: f32): f32 { ret x; }

  fn main(): f32
  {
    ret 0;
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, FunctionDefinitionsThrows)
{
  std::string source = R"SRC(
  fn f(x: f32): f32 { ret 1; }
  fn g(x: f32, y: f32): f32 { ret x + y; }

  fn main(): f32
  {
    ret 0;
  }
  )SRC";
  nothrowTest(source);
}

TEST(SemanticAnalyserTest, ShadowingWorks)
{
  std::string source = R"SRC(
  
  let test: function = \(x: f32): f32 = { ret x + 1; };

  fn main(): f32
  {
    let test: f32 = 12;
    let x: f32 = 12 * test;

    ret 0;
  }
  )SRC";
  nothrowTest(source);
}

TEST(SemanticAnalyserTest, BuildInFunctionPrintDefined)
{
  std::string source = R"SRC(
  fn main(): f32
  {
    print("test");
    ret 0;
  }
  )SRC";
  nothrowTest(source);
}

TEST(SemanticAnalyserTest, BuildInFunctionIfDefined)
{
  std::string source = R"SRC(
  fn main(): f32
  {
    let x: f32 = if(1 == 1, 1, 0);
    ret 0;
  }
  )SRC";
  nothrowTest(source);
}

TEST(SemanticAnalyserTest, UndeclaredVariableAccessInDeclarationThrows)
{
  std::string source = R"SRC(
  fn main(): f32
  {
    let x: f32 = 2*y;
    ret 0;
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, VariableAccessWorks)
{
  std::string source = R"SRC(
  let global: f32 = 23;
  
  fn main(): f32
  {
    let x: f32 = 2*global;
    let y: f32 = x;
    ret 0;
  }
  )SRC";
  nothrowTest(source);
}

TEST(SemanticAnalyserTest, AssignmentWorks)
{
  std::string source = R"SRC(
  fn main(): f32
  {
    let x: f32 = 2;
    x = 1;
    x += 2;
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, AssignmentToUndeclaredVariableThrows)
{
  std::string source = R"SRC(
  fn main(): f32
  {
    x = 2;
    ret 0;
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, UndeclaredVariableAccessInAssignmentThrows)
{
  std::string source = R"SRC(
  fn main(): f32
  {
    let x: f32 = 42;
    x = 2*y;
    ret 0;
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, UndeclaredVariableAccessInArgumentThrows)
{
  std::string source = R"SRC(
  fn test(x: f32, y: f32): void
  {
    print("x = " : x : " y = " : y);
  }
  
  fn main(): f32
  {
    let x: f32 = 12;
    test(x, y);
    ret 0;
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, VariableAccessInArgumentWorks)
{
  std::string source = R"SRC(
  fn test(x: f32, y: f32): void
  {
    print("x = " : x : " y = " : y);
  }
  
  fn main(): f32
  {
    let x: f32 = 12;
    let y: f32 = 14;
    test(x, y);
    ret 0;
  }
  )SRC";
  nothrowTest(source);
}

TEST(SemanticAnalyserTest, UndeclaredVariableInReturnThrows)
{
  std::string source = R"SRC(  
  fn test(): f32
  {
    ret x;
  }

  fn main(): f32
  {
    ret 0;
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, UndeclaredFunctionCallThrows)
{
  std::string source = R"SRC(
  fn main(): f32
  {
    let x: f32 = 12;
    test(x);
    ret 0;
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, UndeclaredFunctionCallInArgumentThrows)
{
  std::string source = R"SRC(
  fn f(): f32 { ret 5; }

  fn main(): f32
  {
    let x: f32 = 12;
    f(test(x));
    ret 0;
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, UndeclaredFunctionCallInReturnThrows)
{
  std::string source = R"SRC(
  fn f(): f32 { ret test(); }

  fn main(): f32
  {
    ret 0;
  }
  )SRC";
  throwTest(source);
}


TEST(SemanticAnalyserTest, UndeclaredFunctionCallInDeclarationThrows)
{
  std::string source = R"SRC(
  fn main(): f32
  {
    let x: f32 = test(12);
    ret 0;
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, UndeclaredFunctionCallInAssignmentThrows)
{
  std::string source = R"SRC(
  fn main(): f32
  {
    let x: f32 = 42;
    x = test(12);
    ret 0;
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, MissingFunctionArgumentInCallThrows)
{
  std::string source = R"SRC(
  fn test(x: f32, y: f32): f32
  {
    ret x + y;
  }

  fn main(): f32
  {
    let x: f32 = 12;
    test(x);
    ret 0;
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, ToManyFunctionArgumentInCallThrows)
{
  std::string source = R"SRC(
  fn test(x: f32, y: f32): f32
  {
    ret x + y;
  }

  fn main(): f32
  {
    test(1, 2, 3);
    ret 0;
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, CallArgumentTypeMismatchThrows)
{
  std::string source = R"SRC(
  fn callTwice(f: function): void
  {
    f();
    f();
  }

  fn main(): f32
  {
    callTwice(12);
    ret 0;
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, CallArgumentTypeMismatchThrows2)
{
  std::string source = R"SRC(
  fn addOne(x: f32): f32
  {
    ret x+1;
  }

  fn main(): f32
  {
    addOne(\(x:f32):f32 = { ret x; });
    ret 0;
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, VoidFunctionReturningValueThrows)
{
  std::string source = R"SRC(
  fn func(): void
  {
    ret 12;
  }

  fn main(): f32
  {
    ret 0;
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, MissingReturnThrows)
{
  std::string source = R"SRC(
  fn func(): f32
  {
  }

  fn main(): f32
  {
   ret 0; 
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, VariableDeclarationTypeMismatchThrows)
{
  std::string source = R"SRC(
  fn main(): f32
  {
    let x: f32 = \(x: f32): void = { };
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, VariableDeclarationTypeMismatchThrows2)
{
  std::string source = R"SRC(
  fn main(): f32
  {
    let x: function = 12;
    ret 0;
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, VariableDeclarationTypeMismatchThrows3)
{
  std::string source = R"SRC(
  fn f(): function
  {
    ret \(x: f32): void = { };
  }
  
  fn main(): f32
  {
    let x: f32 = f();
    ret 0;
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, VariableDeclarationTypeMismatchThrows4)
{
  std::string source = R"SRC(
  fn f(): f32
  {
    ret 12;
  }
  
  fn main(): f32
  {
    let x: function = f();
    ret 0;
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, VariableAssignmentTypeMismatchThrows)
{
  std::string source = R"SRC(
  fn main(): f32
  {
    let x: f32 = 42;
    x = \(x: f32): void = { };
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, VariableAssignmentTypeMismatchThrows2)
{
  std::string source = R"SRC(
  fn main(): f32
  {
    let x: function = \(x: f32): void = { };
    x = 12;
    ret 0;
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, VariableAssignmentTypeMismatchThrows3)
{
  std::string source = R"SRC(
  fn f(): function
  {
    ret \(x: f32): void = { };
  }
  
  fn main(): f32
  {
    let x: f32 = 12;
    x = f();
    ret 0;
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, VariableAssignmentTypeMismatchThrows4)
{
  std::string source = R"SRC(
  fn f(): f32
  {
    ret 12;
  }
  
  fn main(): f32
  {
    let x: function = \(x: f32): void = { };
    x = f();
    ret 0;
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, MissingMainThrows)
{
  std::string source = R"SRC(
  fn test(): void
  {
    print("Test");
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, InvalidFunctionResultCallThrows)
{
  std::string source = R"SRC(
  fn f(x: f32, y: f32): f32
  {
    ret x + y;
  }
  
  fn main(): f32
  {
    f(1, 2)(23, 5);
    ret 0;
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, InvalidLambdaResultCallThrows)
{
  std::string source = R"SRC(  
  fn main(): f32
  {
    (\(x: f32): function = { ret 12; })(1);
    ret 0;
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, InvalidVariableCallThrows)
{
  std::string source = R"SRC(  
  fn main(): f32
  {
    let x: f32 = 2*2 - 4;
    x(1, 2);
    ret 0;
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, FunctionResultCallWorks)
{
  std::string source = R"SRC(
  fn f(x: f32): function
  {
    ret \(m: f32): f32 = { ret x*m; };
  }
  
  fn main(): f32
  {
    f(1)(23);
    ret 0;
  }
  )SRC";
  nothrowTest(source);
}

TEST(SemanticAnalyserTest, LambdaResultCallWorks)
{
  std::string source = R"SRC(  
  fn main(): f32
  {
    (\(x: f32): function = { ret \(m: f32): f32 = { ret 2; }; })(1)(2);
    ret 0;
  }
  )SRC";
  nothrowTest(source);
}

TEST(SemanticAnalyserTest, VariableCallWorks)
{
  std::string source = R"SRC(  
  fn main(): f32
  {
    let x: function = print;
    print("test");
    ret 0;
  }
  )SRC";
  nothrowTest(source);
}

TEST(SemanticAnalyserTest, InvalidUnaryOperationsThrow)
{
  std::string source = R"SRC(  
  fn main(): f32
  {
    let x: function = print;
    let t: function = -print;
  }
  )SRC";
  throwTest(source);
}

TEST(SemanticAnalyserTest, InvalidBinaryOperationsThrow)
{
  std::string source = R"SRC(  
  fn main(): f32
  {
    let x: function = \(x: f32): f32 = { ret x; };
    let y: f32 = 12;
    let z: f32 = y + x;
  }
  )SRC";
  throwTest(source);
}