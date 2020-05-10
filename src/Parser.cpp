#include "Parser.hpp"

#include "Node.hpp"
#include "PrintVisitor.hpp"

#include <stdexcept>

Parser::Parser(std::istream& stream): tokenizer_(stream) {}

[[noreturn]]
void Parser::reportError(const std::string& msg) const
{
  const auto token = tokenizer_.peek();
  const auto mark = token.mark;
  std::stringstream ss;
  ss << "ERROR (" << mark.to_string() << "): " << msg;
  throw std::runtime_error(ss.str());
}

void Parser::expectToken(TokenType type, const std::string& msg)
{
  const auto token = tokenizer_.peek();
  if(token.type == type)
    tokenizer_.nextToken();
  else
    reportError(msg);
}

Token Parser::getToken(TokenType type, const std::string& msg)
{
  const auto token = tokenizer_.peek();
  if(token.type == type)
    tokenizer_.nextToken();
  else
    reportError(msg);

  return token;
}

std::unique_ptr<ExpressionNode> 
Parser::parseExpression(std::function<std::unique_ptr<ExpressionNode>()> parseOperand, 
  std::function<bool(const Token&)> operatorPredicate)
{
  auto node = parseOperand();
  auto token = tokenizer_.peek();
  while(operatorPredicate(token))
  {
    tokenizer_.nextToken();
    auto left = std::move(node);
    auto op = binaryOperatorFromToken(token);
    auto right = parseOperand();
    node = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    token = tokenizer_.peek();
  }
  return node;
}


std::unique_ptr<Node> Parser::parseProgram()
{
  auto programNode = std::make_unique<ProgramNode>();
  auto token = tokenizer_.peek();
  while(token.type == TokenType::KeywordFn || token.type == TokenType::KeywordLet)
  {
    if(token.type == TokenType::KeywordFn)
    {
      auto functionDeclNode = parseFunctionDeclaration();
      programNode->addFunction(std::move(functionDeclNode));
    }
    else if(token.type == TokenType::KeywordLet)
    {
      auto variableDeclNode = parseVariableDeclaration();
      programNode->addVariable(std::move(variableDeclNode));
    }
    token = tokenizer_.peek();
  }
  expectToken(TokenType::EOT, "Unexpected token!");
  return programNode;
}

std::unique_ptr<ExpressionNode> Parser::parseStringExpression()
{
  const auto str = std::get<std::string>(getToken(TokenType::String, "Expected string!").value);
  std::unique_ptr<ExpressionNode> node = std::make_unique<StringLiteralNode>(str);

  auto token = tokenizer_.peek();
  while(token.type == TokenType::Colon)
  {
    token = tokenizer_.nextToken();
    if(token.type == TokenType::String)
    {
      auto left = std::move(node);
      auto right = std::make_unique<StringLiteralNode>(std::get<std::string>(token.value));
      node = std::make_unique<BinaryOpNode>(std::move(left), BinaryOperator::Addition, std::move(right));
      token = tokenizer_.nextToken();
    }
    else
    {
      auto left = std::move(node);
      auto right = parseArithmeticExpression();
      node = std::make_unique<BinaryOpNode>(std::move(left), BinaryOperator::Addition, std::move(right));
      token = tokenizer_.peek();
    }
  }

  return node;
}

std::unique_ptr<ExpressionNode> Parser::parseLogicalExpression()
{
  const auto predicate = [](const Token& token) 
  { 
    return token.type == TokenType::LogicalAnd || 
      token.type == TokenType::LogicalOr; 
  };
  return parseExpression(std::bind(&Parser::parseUnaryLogical, this), predicate);
}

std::unique_ptr<ExpressionNode> Parser::parseUnaryLogical()
{
  const auto token = tokenizer_.peek();
  if(token.type == TokenType::LogicalNot)
  {
    tokenizer_.nextToken();
    const auto op = unaryOperatorFromToken(token);
    auto comparison = parseComparisonExpression();
    return std::make_unique<UnaryNode>(op, std::move(comparison));
  }
  else
    return parseComparisonExpression();
}

std::unique_ptr<ExpressionNode> Parser::parseComparisonExpression()
{
  return parseExpression(std::bind(&Parser::parseArithmeticExpression, this), Token::isComparisonOperator);
}

std::unique_ptr<ExpressionNode> Parser::parseArithmeticExpression()
{
  return parseExpression(std::bind(&Parser::parseAddExpression, this), Token::isBinaryOperator);
}

std::unique_ptr<ExpressionNode> Parser::parseAddExpression()
{
  const auto predicate = [](const Token& token) 
  { 
    return token.type == TokenType::Plus ||
      token.type == TokenType::Minus ||
      token.type == TokenType::Modulo; 
  };
  return parseExpression(std::bind(&Parser::parseFactor, this), predicate);
}

std::unique_ptr<ExpressionNode> Parser::parseFactor()
{
  const auto predicate = [](const Token& token) 
  { 
    return token.type == TokenType::Mul ||
      token.type == TokenType::Div; 
  };
  return parseExpression(std::bind(&Parser::parseUnary, this), predicate);
}

std::unique_ptr<ExpressionNode> Parser::parseUnary()
{
  const auto token = tokenizer_.peek();
  if(token.type == TokenType::Minus || token.type == TokenType::BinaryNot)
  {
    tokenizer_.nextToken();
    const auto op = unaryOperatorFromToken(token);
    auto term = parseTerm();
    return std::make_unique<UnaryNode>(op, std::move(term));
  }
  else
    return parseTerm();
}

std::unique_ptr<ExpressionNode> Parser::parseTerm()
{
  auto token = tokenizer_.peek();
  if(token.type == TokenType::Number)
  {
    tokenizer_.nextToken();
    return std::make_unique<NumericLiteralNode>(std::get<double>(token.value));
  }
  else if(token.type == TokenType::Identifier || Token::isSpecialFunction(token))
  {
    const auto identifierToken = token;
    token = tokenizer_.nextToken();
    if(token.type == TokenType::LParen)
      return parseFunctionCall(identifierToken);
    else
      return std::make_unique<VariableNode>(std::get<std::string>(identifierToken.value));
  }
  else if(token.type == TokenType::LParen)
  {
    token = tokenizer_.nextToken();
    if(token.type == TokenType::Backslash)
      return parseLambdaCall(true);

    auto expr = parseLogicalExpression();
    if(tokenizer_.peek().type == TokenType::RParen)
    {
      tokenizer_.nextToken();
      return expr;
    }
    else
      reportError("Expected closing parenthesis!");
  }
  else
    reportError("Expected number, identifier of function call!");

  return nullptr;
}

std::unique_ptr<ExpressionNode> Parser::parseFunctionCall(std::optional<Token> identifierToken)
{
  std::string name;
  if(identifierToken.has_value())
    name = std::get<std::string>(identifierToken.value().value);
  else if(Token::isSpecialFunction(tokenizer_.peek()))
  {
    name = std::get<std::string>(tokenizer_.peek().value);
    tokenizer_.nextToken();
  }
  else
    name = std::get<std::string>(getToken(TokenType::Identifier, "Expected function name!").value);
  
  auto arguments = parseCallArgumentList();
  std::unique_ptr<ExpressionNode> node = std::make_unique<FunctionCallNode>(name, std::move(arguments));

  auto token = tokenizer_.peek();
  while(token.type == TokenType::LParen)
  {
    arguments = parseCallArgumentList();
    auto func = std::move(node);
    node = std::make_unique<FunctionResultCallNode>(std::move(func), std::move(arguments));
    token = tokenizer_.peek();
  }

  return node;
}

std::unique_ptr<FunctionCallStatementNode> Parser::parseFunctionCallStatement(std::optional<Token> identifierToken)
{
  auto functionCall = parseFunctionCall(identifierToken);
  expectToken(TokenType::Semicolon, "Expected semicolon!");
  return std::make_unique<FunctionCallStatementNode>(std::move(functionCall));
}

std::unique_ptr<FunctionCallStatementNode> Parser::parseLambdaCallStatement()
{
  auto lambdaCall = parseLambdaCall();
  expectToken(TokenType::Semicolon, "Expected semicolon!");
  return std::make_unique<FunctionCallStatementNode>(std::move(lambdaCall));
}

std::unique_ptr<VariableDeclarationNode> Parser::parseVariableDeclaration()
{
  expectToken(TokenType::KeywordLet, "Expected variable declaration!");
  const auto name = std::get<std::string>(getToken(TokenType::Identifier, "Expected variable name!").value);
  expectToken(TokenType::Colon, "Expected colon!");
  const auto type = parseType();
  expectToken(TokenType::Assign, "Expected assigment operator!");

  const auto token = tokenizer_.peek();
  std::unique_ptr<ExpressionNode> value = nullptr;
  if(token.type == TokenType::Backslash)
    value = parseLambda();
  else
    value = parseLogicalExpression();

  expectToken(TokenType::Semicolon, "Expected semicolon!");
  return std::make_unique<VariableDeclarationNode>(name, type, std::move(value));
}

std::unique_ptr<AssignmentNode> Parser::parseAssignment(std::optional<Token> identifierToken)
{
  std::string name;
  std::unique_ptr<ExpressionNode> value = nullptr;

  if(identifierToken.has_value())
    name = std::get<std::string>(identifierToken.value().value);
  else
    name = std::get<std::string>(getToken(TokenType::Identifier, "Expected variable name!").value);  
  
  auto token = tokenizer_.peek();
  auto op = AssignmentOperator::Assign;
  if(Token::isAssigmentOperator(token))
  {
    op = assignmentOperatorFromToken(token);
    token = tokenizer_.nextToken();
    if(op == AssignmentOperator::Assign && token.type == TokenType::Backslash)
      value = parseLambda();
    else
      value = parseArithmeticExpression();
  }
  else
    reportError("Expected assignment operator!");

  expectToken(TokenType::Semicolon, "Expected semicolon!");
  return std::make_unique<AssignmentNode>(name, op, std::move(value));
}

std::unique_ptr<StatementNode> Parser::parseReturnStatement()
{
  expectToken(TokenType::KeywordRet, "Expected return statement!");
  std::unique_ptr<ExpressionNode> value = nullptr;

  if(tokenizer_.peek().type == TokenType::Backslash)
    value = parseLambda();
  else
    value = parseArithmeticExpression();

  expectToken(TokenType::Semicolon, "Expected semicolon!");
  return std::make_unique<ReturnNode>(std::move(value));
}

std::unique_ptr<BlockNode> Parser::parseBlock()
{
  expectToken(TokenType::LBrace, "Expected block statement!");
  auto blockNode = std::make_unique<BlockNode>();
  auto token = tokenizer_.peek();
  while(token.type == TokenType::KeywordRet || 
    token.type == TokenType::KeywordLet || 
    token.type == TokenType::Identifier ||
    token.type == TokenType::LParen ||
    Token::isSpecialFunction(token))
  {
    if(token.type == TokenType::KeywordRet)
    {
      auto retNode = parseReturnStatement();
      blockNode->addStatement(std::move(retNode));
    }
    else if(token.type == TokenType::KeywordLet)
    {
      auto varDeclNode = parseVariableDeclaration();
      blockNode->addStatement(std::move(varDeclNode));
    }
    else if(token.type == TokenType::Identifier)
    {
      const auto identifierToken = token;
      token = tokenizer_.nextToken();
      if(Token::isAssigmentOperator(token))
      {
        auto assignmentNode = parseAssignment(identifierToken);
        blockNode->addStatement(std::move(assignmentNode));
      }
      else
      {
        auto functionCallNode = parseFunctionCallStatement(identifierToken);
        blockNode->addStatement(std::move(functionCallNode));
      }
    }
    else if(token.type == TokenType::LParen)
    {
      auto lambdaCallNode = parseLambdaCallStatement();
      blockNode->addStatement(std::move(lambdaCallNode));
    }
    else if(Token::isSpecialFunction(token))
    {
      auto functionCallNode = parseFunctionCallStatement();
      blockNode->addStatement(std::move(functionCallNode));
    }
    token = tokenizer_.peek();
  }
  expectToken(TokenType::RBrace, "Expected block end!");
  return blockNode;
}

std::unique_ptr<FunctionDeclarationNode> Parser::parseFunctionDeclaration()
{
  expectToken(TokenType::KeywordFn, "Expected function declaration!");
  const auto name = std::get<std::string>(getToken(TokenType::Identifier, "Expected function name!").value);
  const auto args = parseArgumentList();

  expectToken(TokenType::Colon, "Expected colon!");
  const auto type = parseType();

  auto body = parseBlock();
  return std::make_unique<FunctionDeclarationNode>(name, type, args, std::move(body));
}

std::unique_ptr<LambdaNode> Parser::parseLambda()
{
  expectToken(TokenType::Backslash, "Expected lambda declaration!");
  const auto args = parseArgumentList();
  
  expectToken(TokenType::Colon, "Expected colon!");
  const auto type = parseType();

  expectToken(TokenType::Assign, "Expected assignment!");
  auto body = parseBlock();
  return std::make_unique<LambdaNode>(type, args, std::move(body));
}

std::unique_ptr<ExpressionNode> Parser::parseLambdaCall(bool lParenSkipped)
{
  if(!lParenSkipped)
    expectToken(TokenType::LParen, "Expected open parenthesis!");
  
  auto lambda = parseLambda();
  expectToken(TokenType::RParen, "Expected closing parenthesis!");

  auto arguments = parseCallArgumentList();
  std::unique_ptr<ExpressionNode> node = std::make_unique<LambdaCallNode>(std::move(lambda), std::move(arguments));

  auto token = tokenizer_.peek();
  while(token.type == TokenType::LParen)
  {
    arguments = parseCallArgumentList();
    auto func = std::move(node);
    node = std::make_unique<FunctionResultCallNode>(std::move(func), std::move(arguments));
    token = tokenizer_.peek();
  }

  return node;
}

TypeName Parser::parseType()
{
  const auto token = tokenizer_.peek();
  if(Token::isTypeName(token))
  {
    const auto type = typeNameFromToken(token);
    tokenizer_.nextToken();
    return type;
  }
  else
    reportError("Expected type name!");
}

std::list<std::unique_ptr<ExpressionNode>> Parser::parseCallArgumentList()
{
  expectToken(TokenType::LParen, "Expected open parenthesis!");
  std::list<std::unique_ptr<ExpressionNode>> arguments{};
  auto token = tokenizer_.peek();
  if(token.type != TokenType::RParen)
  {
    auto arg = parseCallArgument();
    arguments.push_back(std::move(arg));
    token = tokenizer_.peek();
    while(token.type == TokenType::Comma)
    {
      tokenizer_.nextToken();
      arg = parseCallArgument();
      arguments.push_back(std::move(arg));
      token = tokenizer_.peek();
    }
  }
  expectToken(TokenType::RParen, "Expected closing parenthesis!");

  return arguments;
}

std::unique_ptr<ExpressionNode> Parser::parseCallArgument()
{
  const auto token = tokenizer_.peek();
  if(token.type == TokenType::String)
    return parseStringExpression();
  else if(token.type == TokenType::Backslash)
    return parseLambda();
  else
    return parseLogicalExpression();
}

std::pair<std::string, TypeName> Parser::parseArgument()
{
  const auto name = 
    std::get<std::string>(getToken(TokenType::Identifier, "Expected argument name!").value);
  expectToken(TokenType::Colon, "Expected colon!");
  const auto type = parseType();

  return std::pair<std::string, TypeName>{name, type};
}

std::list<std::pair<std::string, TypeName>> Parser::parseArgumentList()
{
  expectToken(TokenType::LParen, "Expected arguments list!");
  std::list<std::pair<std::string, TypeName>> arguments{};
  auto token = tokenizer_.peek();
  
  if(token.type != TokenType::RParen)
  {
    auto arg = parseArgument();
    arguments.push_back(arg);
    token = tokenizer_.peek();
    while(token.type == TokenType::Comma)
    {
      tokenizer_.nextToken();
      arg = parseArgument();
      arguments.push_back(arg);
      token = tokenizer_.peek();
    }
  }
  expectToken(TokenType::RParen, "Expected closing parenthesis!");

  return arguments;
}

UnaryOperator Parser::unaryOperatorFromToken(const Token& token) const
{
  const auto type = token.type;
  switch(type)
  {
    case TokenType::Minus:
      return UnaryOperator::Minus;
    case TokenType::BinaryNot:
      return UnaryOperator::BinaryNegation;
    case TokenType::LogicalNot:
      return UnaryOperator::LogicalNot;
    default:
      throw std::runtime_error("Unexpected token for unary operator!");
  }
}

BinaryOperator Parser::binaryOperatorFromToken(const Token& token) const
{
  const auto type = token.type;
  switch(type)
  {
    case TokenType::Plus:
      return BinaryOperator::Addition;
    case TokenType::Minus:
      return BinaryOperator::Subtraction;
    case TokenType::Mul:
      return BinaryOperator::Multiplication;
    case TokenType::Div:
      return BinaryOperator::Division;
    case TokenType::Modulo:
      return BinaryOperator::Modulo;
    case TokenType::LogicalAnd:
      return BinaryOperator::LogicalAnd;
    case TokenType::LogicalOr:
      return BinaryOperator::LogicalOr;
    case TokenType::BinaryAnd:
      return BinaryOperator::BinaryAnd;
    case TokenType::BinaryOr:
      return BinaryOperator::BinaryOr;
    case TokenType::BinaryXor:
      return BinaryOperator::BinaryXor;
    case TokenType::ShiftLeft:
      return BinaryOperator::ShiftLeft;
    case TokenType::ShiftRight:
      return BinaryOperator::ShiftRight;
    case TokenType::Greater:
      return BinaryOperator::Greater;
    case TokenType::GreaterOrEqual:
      return BinaryOperator::GreaterEq;
    case TokenType::Less:
      return BinaryOperator::Less;
    case TokenType::LessOrEqual:
      return BinaryOperator::LessEq;
    case TokenType::Equal:
      return BinaryOperator::Equal;
    case TokenType::NotEqual:
      return BinaryOperator::NotEqual;
    default:
      throw std::runtime_error("Unexpected token for binary operator!");
  }
}

AssignmentOperator Parser::assignmentOperatorFromToken(const Token& token) const
{
  const auto type = token.type;
  switch(type)
  {
    case TokenType::Assign:
      return AssignmentOperator::Assign;
    case TokenType::PlusEq:
      return AssignmentOperator::PlusEq;
    case TokenType::MinusEq:
      return AssignmentOperator::MinusEq;
    case TokenType::MulEq:
      return AssignmentOperator::MulEq;
    case TokenType::DivEq:
      return AssignmentOperator::DivEq;
    case TokenType::AndEq:
      return AssignmentOperator::AndEq;
    case TokenType::OrEq:
      return AssignmentOperator::OrEq;
    case TokenType::XorEq:
      return AssignmentOperator::XorEq;
    case TokenType::ShiftLeftEq:
      return AssignmentOperator::ShiftLeftEq;
    case TokenType::ShiftRightEq:
      return AssignmentOperator::ShiftRightEq;
    default:
      throw std::runtime_error("Unexpected token for assignment operator!");
  }
}

TypeName Parser::typeNameFromToken(const Token& token) const
{
  const auto type = token.type;
  switch(type)
  {
    case TokenType::KeywordF32:
      return TypeName::F32;
    case TokenType::KeywordFunction:
      return TypeName::Function;
    case TokenType::KeywordVoid:
      return TypeName::Void;
    default:
      throw std::runtime_error("Unexpected token for type name!");
  }
}
