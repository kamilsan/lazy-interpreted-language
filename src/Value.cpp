#include "Value.h"

std::unique_ptr<Value> Number::clone() const
{
  return std::make_unique<Number>(value_);
}

std::unique_ptr<Value> String::clone() const
{
  return std::make_unique<String>(value_);
}

std::unique_ptr<Value> Function::clone() const
{
  return std::make_unique<Function>(returnType_, arguments_, body_, context_.clone());
}

void NumberValueAnalyser::visit(const Number& num)
{
  valid_ = true;
  value_ = num.getValue();
}

void NumberValueAnalyser::visit(const String&)
{
  valid_ = false;
}

void NumberValueAnalyser::visit(const Function&)
{
  valid_ = false;
}


void StringValueAnalyser::visit(const Number&)
{
  valid_ = false;
}

void StringValueAnalyser::visit(const String& str)
{
  valid_ = true;
  value_ = str.getValue();
}

void StringValueAnalyser::visit(const Function&)
{
  valid_ = false;
}

void FunctionValueAnalyser::visit(const Number&)
{
  valid_ = false;
}

void FunctionValueAnalyser::visit(const String&)
{
  valid_ = false;
}

void FunctionValueAnalyser::visit(const Function& func)
{
  valid_ = true;
  returnType_ = func.getReturnType();
  arguments_ = func.getArguments();
  body_ = func.getBodyPtr();
  context_ = func.getContext();
}