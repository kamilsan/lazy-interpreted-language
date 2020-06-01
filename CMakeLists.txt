cmake_minimum_required(VERSION 2.4)

project(interpreter)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

add_subdirectory(googletest)
include_directories(googletest include)

set(PROJECT_CODE 
  include/Mark.hpp
  include/Token.hpp
  include/Visitor.hpp
  include/AST.hpp
  src/PrintVisitor.cpp include/PrintVisitor.hpp
  src/Symbol.cpp include/Symbol.hpp
  src/TypeChecker.cpp include/TypeChecker.hpp
  src/SemanticAnalyser.cpp include/SemanticAnalyser.hpp
  src/Context.cpp include/Context.hpp
  src/Executor.cpp include/Executor.hpp
  src/Stream.cpp include/Stream.hpp
  src/Tokenizer.cpp include/Tokenizer.hpp
  src/Parser.cpp include/Parser.hpp
  src/Value.cpp include/Value.h
  src/Common.cpp include/Common.hpp)

add_executable(interpreter
  ${PROJECT_CODE}
  src/main.cpp
)

add_executable(interpreter_tests
  ${PROJECT_CODE}
  tests/TokenizerTests.cpp
  tests/PrintVisitorTests.cpp
  tests/ParserTests.cpp
  tests/SemanticAnalyserTests.cpp
  tests/main_test.cpp
        tests/ExecutorTests.cpp)

target_link_libraries(interpreter_tests gtest gtest_main)