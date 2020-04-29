#pragma once

#include <memory>

class Node;

class AST
{
public:
  AST();
private:
  std::unique_ptr<Node> root_;
};