#pragma once

#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

#include "Token.hpp"
#include "Visitor.hpp"

enum class UnaryOperator
{
  BinaryNegation,
  Minus,
  LogicalNot
};

enum class BinaryOperator
{
  Addition,
  Subtraction,
  Multiplication,
  Division,
  Modulo,
  LogicalAnd,
  LogicalOr,
  BinaryAnd,
  BinaryOr,
  BinaryXor,
  ShiftLeft,
  ShiftRight,
  Greater,
  GreaterEq,
  Less,
  LessEq,
  Equal,
  NotEqual
};

enum class AssignmentOperator
{
  Assign,
  PlusEq,
  MinusEq,
  MulEq,
  DivEq,
  AndEq,
  OrEq,
  XorEq,
  ShiftLeftEq,
  ShiftRightEq
};

enum class TypeName
{
  F32,
  Function,
  Void,
  String
};

const std::unordered_map<UnaryOperator, std::string> UnaryOperationNames = {
  std::make_pair<UnaryOperator, std::string>(UnaryOperator::BinaryNegation, "BinaryNegation"),
  std::make_pair<UnaryOperator, std::string>(UnaryOperator::Minus, "Minus"),
  std::make_pair<UnaryOperator, std::string>(UnaryOperator::LogicalNot, "LogicalNot")
};

const std::unordered_map<BinaryOperator, std::string> BinaryOperationNames = {
  std::make_pair<BinaryOperator, std::string>(BinaryOperator::Addition, "Addition"),
  std::make_pair<BinaryOperator, std::string>(BinaryOperator::Subtraction, "Subtraction"),
  std::make_pair<BinaryOperator, std::string>(BinaryOperator::Multiplication, "Multiplication"),
  std::make_pair<BinaryOperator, std::string>(BinaryOperator::Division, "Division"),
  std::make_pair<BinaryOperator, std::string>(BinaryOperator::Modulo, "Modulo"),
  std::make_pair<BinaryOperator, std::string>(BinaryOperator::LogicalAnd, "LogicalAnd"),
  std::make_pair<BinaryOperator, std::string>(BinaryOperator::LogicalOr, "LogicalOr"),
  std::make_pair<BinaryOperator, std::string>(BinaryOperator::BinaryAnd, "BinaryAnd"),
  std::make_pair<BinaryOperator, std::string>(BinaryOperator::BinaryOr, "BinaryOr"),
  std::make_pair<BinaryOperator, std::string>(BinaryOperator::BinaryXor, "BinaryXor"),
  std::make_pair<BinaryOperator, std::string>(BinaryOperator::ShiftLeft, "ShiftLeft"),
  std::make_pair<BinaryOperator, std::string>(BinaryOperator::ShiftRight, "ShiftRight"),
  std::make_pair<BinaryOperator, std::string>(BinaryOperator::Greater, "Greater"),
  std::make_pair<BinaryOperator, std::string>(BinaryOperator::GreaterEq, "GreaterEq"),
  std::make_pair<BinaryOperator, std::string>(BinaryOperator::Less, "Less"),
  std::make_pair<BinaryOperator, std::string>(BinaryOperator::LessEq, "LessEq"),
  std::make_pair<BinaryOperator, std::string>(BinaryOperator::Equal, "Equal"),
  std::make_pair<BinaryOperator, std::string>(BinaryOperator::NotEqual, "NotEqual")
};

const std::unordered_map<AssignmentOperator, std::string> AssignmentOperationNames = {
  std::make_pair<AssignmentOperator, std::string>(AssignmentOperator::Assign, "Assign"),
  std::make_pair<AssignmentOperator, std::string>(AssignmentOperator::PlusEq, "PlusEq"),
  std::make_pair<AssignmentOperator, std::string>(AssignmentOperator::MinusEq, "MinusEq"),
  std::make_pair<AssignmentOperator, std::string>(AssignmentOperator::MulEq, "MulEq"),
  std::make_pair<AssignmentOperator, std::string>(AssignmentOperator::DivEq, "DivEq"),
  std::make_pair<AssignmentOperator, std::string>(AssignmentOperator::AndEq, "AndEq"),
  std::make_pair<AssignmentOperator, std::string>(AssignmentOperator::OrEq, "OrEq"),
  std::make_pair<AssignmentOperator, std::string>(AssignmentOperator::XorEq, "XorEq"),
  std::make_pair<AssignmentOperator, std::string>(AssignmentOperator::ShiftLeftEq, "ShiftLeftEq"),
  std::make_pair<AssignmentOperator, std::string>(AssignmentOperator::ShiftRightEq, "ShiftRightEq")
};

const std::unordered_map<TypeName, std::string> TypeNameStrings = {
  std::make_pair<TypeName, std::string>(TypeName::F32, "f32"),
  std::make_pair<TypeName, std::string>(TypeName::Function, "function"),
  std::make_pair<TypeName, std::string>(TypeName::Void, "void"),
  std::make_pair<TypeName, std::string>(TypeName::String, "string")
};

class Node
{
public:
  virtual void accept(Visitor&) const = 0;
};

class ExpressionNode : public Node
{
public:
  virtual void accept(Visitor&) const = 0;
};

class StatementNode : public Node
{
public:
  virtual void accept(Visitor&) const = 0;
};

class ProgramNode : public Node
{
public:
  ProgramNode(): variables_(), functions_() {}

  void addVariable(std::unique_ptr<VariableDeclarationNode> variable) 
    { variables_.push_back(std::move(variable)); }
  void addFunction(std::unique_ptr<FunctionDeclarationNode> function) 
    { functions_.push_back(std::move(function)); }

  const std::list<std::unique_ptr<VariableDeclarationNode>>& getVariables() 
    const { return variables_; }
  const std::list<std::unique_ptr<FunctionDeclarationNode>>& getFunctions() 
    const { return functions_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  std::list<std::unique_ptr<VariableDeclarationNode>> variables_;
  std::list<std::unique_ptr<FunctionDeclarationNode>> functions_;
};

class NumericLiteralNode : public ExpressionNode
{
public:
  NumericLiteralNode(double value): value_(value) {}

  double getValue() const { return value_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  double value_;
};

class StringLiteralNode : public ExpressionNode
{
public:
  StringLiteralNode(const std::string& value): value_(value) {}

  const std::string& getValue() const { return value_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  std::string value_;
};

class VariableNode : public ExpressionNode
{
public:
  VariableNode(const std::string& name): name_(name) {}

  std::string getName() const { return name_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  std::string name_;
};

class UnaryNode : public ExpressionNode
{
public:
  UnaryNode(const UnaryOperator& unaryOp, std::unique_ptr<ExpressionNode> term):
    unaryOperator_(unaryOp), term_(std::move(term)) {}

  const ExpressionNode& getTerm() const { return *term_; }
  const UnaryOperator& getOperation() const { return unaryOperator_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  UnaryOperator unaryOperator_;
  std::unique_ptr<ExpressionNode> term_;
};

class BinaryOpNode : public ExpressionNode
{
public:
  BinaryOpNode(std::unique_ptr<ExpressionNode> leftOperand, const BinaryOperator& op, std::unique_ptr<ExpressionNode> rightOperand):
    leftOperand_(std::move(leftOperand)), operator_(op), rightOperand_(std::move(rightOperand)) {}

  const ExpressionNode& getLeftOperand() const { return *leftOperand_; }
  const ExpressionNode& getRightOperand() const { return *rightOperand_; }
  const BinaryOperator& getOperation() const { return operator_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  std::unique_ptr<ExpressionNode> leftOperand_;
  BinaryOperator operator_;
  std::unique_ptr<ExpressionNode> rightOperand_;
};

class FunctionResultCallNode : public ExpressionNode
{
public:
  FunctionResultCallNode(std::unique_ptr<ExpressionNode> call, std::list<std::unique_ptr<ExpressionNode>> arguments):
    call_(std::move(call)), arguments_(std::move(arguments)) {}

  const ExpressionNode& getCall() const { return *call_; }
  const std::list<std::unique_ptr<ExpressionNode>>& getArguments() const { return arguments_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  std::unique_ptr<ExpressionNode> call_;
  std::list<std::unique_ptr<ExpressionNode>> arguments_;
};

class FunctionCallNode : public ExpressionNode
{
public:
  FunctionCallNode(const std::string& name, std::list<std::unique_ptr<ExpressionNode>> arguments):
    name_(name), arguments_(std::move(arguments)) {}

  const std::string& getName() const { return name_; }
  const std::list<std::unique_ptr<ExpressionNode>>& getArguments() const { return arguments_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  std::string name_;
  std::list<std::unique_ptr<ExpressionNode>> arguments_;
};

class LambdaNode : public ExpressionNode
{
public:
  LambdaNode(const TypeName& returnType, 
    const std::list<std::pair<std::string, TypeName>> args, std::unique_ptr<BlockNode> body):
      returnType_(returnType), arguments_(args), body_(std::move(body)) {}

  const TypeName& getReturnType() const { return returnType_; }
  const std::list<std::pair<std::string, TypeName>>& getArguments() const { return arguments_; }
  const BlockNode& getBody() const { return *body_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  TypeName returnType_;
  std::list<std::pair<std::string, TypeName>> arguments_;
  std::unique_ptr<BlockNode> body_;
};

class LambdaCallNode : public ExpressionNode
{
public:
  LambdaCallNode(std::unique_ptr<LambdaNode> lambda, std::list<std::unique_ptr<ExpressionNode>> arguments):
    lambda_(std::move(lambda)), arguments_(std::move(arguments)) {}

  const LambdaNode& getLambda() const { return *lambda_; }
  const std::list<std::unique_ptr<ExpressionNode>>& getArguments() const { return arguments_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  std::unique_ptr<LambdaNode> lambda_;
  std::list<std::unique_ptr<ExpressionNode>> arguments_;
};

class VariableDeclarationNode : public StatementNode
{
public:
  VariableDeclarationNode(const std::string& name, const TypeName& type, std::unique_ptr<ExpressionNode> value):
    name_(name), type_(type), value_(std::move(value)) {}

  const std::string& getName() const { return name_; }
  const TypeName& getType() const { return type_; }
  const ExpressionNode& getValue() const { return *value_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  std::string name_;
  TypeName type_;
  std::unique_ptr<ExpressionNode> value_;
};

class AssignmentNode : public StatementNode
{
public:
  AssignmentNode(const std::string& name, const AssignmentOperator& operation, std::unique_ptr<ExpressionNode> value):
    name_(name), operator_(operation), value_(std::move(value)) {}

  const std::string& getName() const { return name_; }
  const AssignmentOperator& getOperation() const { return operator_; }
  const ExpressionNode& getValue() const { return *value_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  std::string name_;
  AssignmentOperator operator_;
  std::unique_ptr<ExpressionNode> value_;
};

class ReturnNode : public StatementNode
{
public:
  ReturnNode(std::unique_ptr<ExpressionNode> value):
    value_(std::move(value)) {}

  const ExpressionNode& getValue() const { return *value_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  std::unique_ptr<ExpressionNode> value_;
};

class BlockNode : public StatementNode
{
public:
  BlockNode(): statements_{} {}

  void addStatement(std::unique_ptr<StatementNode> statement) { statements_.push_back(std::move(statement)); }
  const std::list<std::unique_ptr<StatementNode>>& getStatements() const { return statements_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  std::list<std::unique_ptr<StatementNode>> statements_;  
};

class FunctionDeclarationNode : public StatementNode
{
public:
  FunctionDeclarationNode(const std::string& name, const TypeName& returnType, 
    const std::list<std::pair<std::string, TypeName>> args, std::unique_ptr<BlockNode> body):
      name_(name), returnType_(returnType), arguments_(args), body_(std::move(body)) {}

  const std::string& getName() const { return name_; }
  const TypeName& getReturnType() const { return returnType_; }
  const std::list<std::pair<std::string, TypeName>>& getArguments() const { return arguments_; }
  const BlockNode& getBody() const { return *body_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  std::string name_;
  TypeName returnType_;
  std::list<std::pair<std::string, TypeName>> arguments_;
  std::unique_ptr<BlockNode> body_;
};

class FunctionCallStatementNode : public StatementNode
{
public:
  FunctionCallStatementNode(std::unique_ptr<ExpressionNode> functionCallNode):
    functionCall_(std::move(functionCallNode)) {}

  const ExpressionNode& getFunctionCall() const { return *functionCall_; }

  void accept(Visitor& visitor) const override { visitor.visit(*this); }
private:
  std::unique_ptr<ExpressionNode> functionCall_;
};
