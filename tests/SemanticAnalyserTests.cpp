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