#pragma once

#include "AST.hpp"

#include <string>

class Number;
class String;
class Function;

class ValueVisitor
{
public:

  virtual void visit(Number&) = 0;
  virtual void visit(String&) = 0;
  virtual void visit(Function&) = 0;

};

class Value
{
public:
  Value(const TypeName& type): valueType_(type) {}

  const TypeName& getType() const { return valueType_; }

  virtual std::unique_ptr<Value> clone() const = 0;
  virtual void accept(ValueVisitor& visitor) = 0;
private:
  TypeName valueType_;
};

class Number : public Value
{
public:
  Number(double value): Value(TypeName::F32), value_(value) {}

  double getValue() const { return value_; }

  std::unique_ptr<Value> clone() const override;
  void accept(ValueVisitor& visitor) override { visitor.visit(*this); }
private:
  double value_;
};

class String : public Value
{
public:
  String(const std::string& value): Value(TypeName::String), value_(value) {}

  const std::string& getValue() const { return value_; }

  std::unique_ptr<Value> clone() const override;
  void accept(ValueVisitor& visitor) override { visitor.visit(*this); }
private:
  std::string value_;
};

class Function : public Value
{
public:
  using ArgumentsList = std::list<std::pair<std::string, TypeName>>;

  Function(const TypeName& returnType, const ArgumentsList& arguments, std::shared_ptr<BlockNode> body):
    Value(TypeName::Function), returnType_(returnType), arguments_(arguments), body_(body) {}

  const TypeName& getReturnType() const { return returnType_; }
  const ArgumentsList & getArguments() const { return arguments_; }
  const BlockNode& getBody() const { return *body_; }
  const std::shared_ptr<BlockNode>& getBodyPtr() const { return body_; }

  std::unique_ptr<Value> clone() const override;
  void accept(ValueVisitor& visitor) override { visitor.visit(*this); }
private:
  TypeName returnType_;
  ArgumentsList arguments_;
  std::shared_ptr<BlockNode> body_;
};

class NumberValueAnalyser : public ValueVisitor
{
public:
  NumberValueAnalyser(): valid_(false), value_() {}

  bool isValid() const { return valid_; }
  const std::optional<double>& getValue() const { return value_; }

  void visit(Number&) override;
  void visit(String&) override;
  void visit(Function&) override;
private:
  bool valid_;
  std::optional<double> value_;
};

class StringValueAnalyser : public ValueVisitor
{
public:
  StringValueAnalyser(): valid_(false), value_() {}

  bool isValid() const { return valid_; }
  const std::optional<std::string>& getValue() const { return value_; }

  void visit(Number&) override;
  void visit(String&) override;
  void visit(Function&) override;
private:
  bool valid_;
  std::optional<std::string> value_;
};