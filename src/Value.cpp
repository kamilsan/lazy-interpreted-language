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
  return nullptr;
 // return std::make_unique<Function>(returnType_, arguments_, body_);
}


void NumberValueAnalyser::visit(Number& num)
{
  valid_ = true;
  value_ = num.getValue();
}

void NumberValueAnalyser::visit(String&)
{
  valid_ = false;
}

void NumberValueAnalyser::visit(Function&)
{
  valid_ = false;
}


void StringValueAnalyser::visit(Number&)
{
  valid_ = false;
}

void StringValueAnalyser::visit(String& str)
{
  valid_ = true;
  value_ = str.getValue();
}

void StringValueAnalyser::visit(Function&)
{
  valid_ = false;
}