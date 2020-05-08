#include "Parser.hpp"

#include "Node.hpp"
#include "PrintVisitor.hpp"

#include <stdexcept>

Parser::Parser(std::istream& stream): tokenizer_(stream) {}

void Parser::expectToken(TokenType type, const std::string& msg)
{
  auto token = tokenizer_.peek();
  if(token.type == type)
    tokenizer_.nextToken();
  else
    throw std::runtime_error(msg);
}

Token Parser::getToken(TokenType type, const std::string& msg)
{
  auto token = tokenizer_.peek();
  if(token.type == type)
    tokenizer_.nextToken();
  else
    throw std::runtime_error(msg);

  return token;
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
  auto str = std::get<std::string>(getToken(TokenType::String, "Expected string!").value);
  std::unique_ptr<ExpressionNode> node = std::make_unique<StringLiteralNode>(str);

  auto token = tokenizer_.peek();
  while(token.type == TokenType::Colon)
  {
    token = tokenizer_.nextToken();
    if(token.type == TokenType::String)
    {
      auto left = std::move(node);
      auto right = std::make_unique<StringLiteralNode>(std::get<std::string>(token.value));
      node = std::make_unique<BinaryOpNode>(std::move(left), BinaryOperation::Addition, std::move(right));
      token = tokenizer_.nextToken();
    }
    else
    {
      auto left = std::move(node);
      auto right = parseArithmeticExpression();
      node = std::make_unique<BinaryOpNode>(std::move(left), BinaryOperation::Addition, std::move(right));
      token = tokenizer_.peek();
    }
  }

  return node;
}

std::unique_ptr<ExpressionNode> Parser::parseLogicalExpression()
{
  auto node = parseUnaryLogical();
  auto token = tokenizer_.peek();
  while(token.type == TokenType::LogicalAnd || token.type == TokenType::LogicalOr)
  {
    tokenizer_.nextToken();
    auto left = std::move(node);
    auto op = binaryOperationFromToken(token);
    auto right = parseUnaryLogical();
    node = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    token = tokenizer_.peek();
  }  
  return node;
}

std::unique_ptr<ExpressionNode> Parser::parseUnaryLogical()
{
  auto token = tokenizer_.peek();
  if(token.type == TokenType::LogicalNot)
  {
    tokenizer_.nextToken();
    auto op = unaryOperationFromToken(token);
    auto comparison = parseComparisonExpression();
    return std::make_unique<UnaryNode>(op, std::move(comparison));
  }
  else
    return parseComparisonExpression();
}

std::unique_ptr<ExpressionNode> Parser::parseComparisonExpression()
{
  auto node = parseArithmeticExpression();
  auto token = tokenizer_.peek();
  while(Token::isComparisonOperator(token))
  {
    tokenizer_.nextToken();
    auto left = std::move(node);
    auto op = binaryOperationFromToken(token);
    auto right = parseArithmeticExpression();
    node = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    token = tokenizer_.peek();
  }
  return node;
}

std::unique_ptr<ExpressionNode> Parser::parseArithmeticExpression()
{
  auto node = parseAddExpression();
  auto token = tokenizer_.peek();
  while(Token::isBinaryOperator(token))
  {
    tokenizer_.nextToken();
    auto left = std::move(node);
    auto op = binaryOperationFromToken(token);
    auto right = parseAddExpression();
    node = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    token = tokenizer_.peek();
  }
  return node;
}

std::unique_ptr<ExpressionNode> Parser::parseAddExpression()
{
  auto node = parseFactor();
  auto token = tokenizer_.peek();
  while(token.type == TokenType::Plus || token.type == TokenType::Minus ||
    token.type == TokenType::Modulo)
  {
    tokenizer_.nextToken();
    auto left = std::move(node);
    auto op = binaryOperationFromToken(token);
    auto right = parseFactor();
    node = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    token = tokenizer_.peek();
  }  
  return node;
}

std::unique_ptr<ExpressionNode> Parser::parseFactor()
{
  auto node = parseUnary();
  auto token = tokenizer_.peek();
  while(token.type == TokenType::Mul || token.type == TokenType::Div)
  {
    tokenizer_.nextToken();
    auto left = std::move(node);
    auto op = binaryOperationFromToken(token);
    auto right = parseUnary();
    node = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    token = tokenizer_.peek();
  }
  return node;
}

std::unique_ptr<ExpressionNode> Parser::parseUnary()
{
  auto token = tokenizer_.peek();
  if(token.type == TokenType::Minus || token.type == TokenType::BinaryNot)
  {
    tokenizer_.nextToken();
    auto op = unaryOperationFromToken(token);
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
  else if(token.type == TokenType::Identifier)
  {
    auto identifierToken = token;
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
      throw std::runtime_error("Expected closing parenthesis!");
  }
  return nullptr;
}

std::unique_ptr<ExpressionNode> Parser::parseFunctionCall(std::optional<Token> identifierToken)
{
  std::string name;
  if(identifierToken.has_value())
    name = std::get<std::string>(identifierToken.value().value);
  else
    name = std::get<std::string>(getToken(TokenType::Identifier, "Expected function name!").value);
  
  auto arguments = std::move(parseCallArgumentList());
  return std::make_unique<FunctionCallNode>(name, std::move(arguments));
}

std::unique_ptr<FunctionCallStatementNode> Parser::parseFunctionCallStatement()
{
  auto functionCall = parseFunctionCall();
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
  auto name = std::get<std::string>(getToken(TokenType::Identifier, "Expected variable name!").value);
  expectToken(TokenType::Colon, "Expected colon!");
  auto token = tokenizer_.peek();
  if(Token::isTypeName(token))
  {
    auto type = typeNameFromToken(token);
    tokenizer_.nextToken();
    expectToken(TokenType::Assign, "Expected assigment operator!");

    auto value = parseLogicalExpression();
    expectToken(TokenType::Semicolon, "Expected semicolon!");

    return std::make_unique<VariableDeclarationNode>(name, type, std::move(value));
  }
  else
    throw std::runtime_error("Expected type name!");
}

std::unique_ptr<StatementNode> Parser::parseReturnStatement()
{
  expectToken(TokenType::KeywordRet, "Expected return statement!");
  auto value = parseArithmeticExpression();
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
    token.type == TokenType::LParen)
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
      auto functionCallNode = parseFunctionCallStatement();
      blockNode->addStatement(std::move(functionCallNode));
    }
    else if(token.type == TokenType::LParen)
    {
      auto lambdaCallNode = parseLambdaCallStatement();
      blockNode->addStatement(std::move(lambdaCallNode));
    }
    token = tokenizer_.peek();
  }
  expectToken(TokenType::RBrace, "Expected block end!");
  return blockNode;
}

std::unique_ptr<FunctionDeclarationNode> Parser::parseFunctionDeclaration()
{
  expectToken(TokenType::KeywordFn, "Expected function declaration!");
  auto name = std::get<std::string>(getToken(TokenType::Identifier, "Expected function name!").value);
  auto args = parseArgumentList();
  expectToken(TokenType::Colon, "Expected colon!");
  auto token = tokenizer_.peek();
  if(Token::isTypeName(token))
  {
    auto type = typeNameFromToken(token);
    tokenizer_.nextToken();

    auto body = parseBlock();
    return std::make_unique<FunctionDeclarationNode>(name, type, args, std::move(body));
  }
  else
    throw std::runtime_error("Expected type name!");
}

std::unique_ptr<LambdaNode> Parser::parseLambda()
{
  expectToken(TokenType::Backslash, "Expected lambda declaration!");
  auto args = parseArgumentList();
  expectToken(TokenType::Colon, "Expected colon!");
  auto token = tokenizer_.peek();
  if(Token::isTypeName(token))
  {
    auto type = typeNameFromToken(token);
    tokenizer_.nextToken();
    expectToken(TokenType::Assign, "Expected assignment!");
    auto body = parseBlock();
    return std::make_unique<LambdaNode>(type, args, std::move(body));
  }
  else
    throw std::runtime_error("Expected type name!");
}

std::unique_ptr<LambdaCallNode> Parser::parseLambdaCall(bool lParenSkipped)
{
  if(!lParenSkipped)
    expectToken(TokenType::LParen, "Expected open parenthesis!");
  
  auto lambda = parseLambda();
  expectToken(TokenType::RParen, "Expected closing parenthesis!");

  auto arguments = std::move(parseCallArgumentList());
  return std::make_unique<LambdaCallNode>(std::move(lambda), std::move(arguments));
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
  auto token = tokenizer_.peek();
  if(token.type == TokenType::String)
    return parseStringExpression();
  else if(token.type == TokenType::Backslash)
    return parseLambda();
  else
    return parseLogicalExpression();
}

std::list<std::pair<std::string, TypeName>> Parser::parseArgumentList()
{
  expectToken(TokenType::LParen, "Expected arguments list!");
  std::list<std::pair<std::string, TypeName>> args{};
  auto token = tokenizer_.peek();
  
  if(token.type == TokenType::Identifier)
  {
    const auto name = std::get<std::string>(token.value);
    tokenizer_.nextToken();
    expectToken(TokenType::Colon, "Expected colon!");
    token = tokenizer_.peek();
    if(Token::isTypeName(token))
    {
      auto type = typeNameFromToken(token);
      args.push_back(std::pair<std::string, TypeName>{name, type});
      token = tokenizer_.nextToken();
    }
    else
      throw std::runtime_error("Expected type name!");

    while(token.type == TokenType::Comma)
    {
      tokenizer_.nextToken();
      const auto name = 
        std::get<std::string>(getToken(TokenType::Identifier, "Expected argument name!").value);
      expectToken(TokenType::Colon, "Expected colon!");
      token = tokenizer_.peek();
      if(Token::isTypeName(token))
      {
        auto type = typeNameFromToken(token);
        args.push_back(std::pair<std::string, TypeName>{name, type});
        token = tokenizer_.nextToken();
      }
      else
        throw std::runtime_error("Expected type name!");
    }
  }
  expectToken(TokenType::RParen, "Expected closing parenthesis!");
  return args;
}

UnaryOperation Parser::unaryOperationFromToken(const Token& token) const
{
  auto type = token.type;
  switch(type)
  {
    case TokenType::Minus:
      return UnaryOperation::Minus;
    case TokenType::BinaryNot:
      return UnaryOperation::BinaryNegation;
    case TokenType::LogicalNot:
      return UnaryOperation::LogicalNot;
    default:
      throw std::runtime_error("Unexpected token for unary operation!");
  }
}

BinaryOperation Parser::binaryOperationFromToken(const Token& token) const
{
  auto type = token.type;
  switch(type)
  {
    case TokenType::Plus:
      return BinaryOperation::Addition;
    case TokenType::Minus:
      return BinaryOperation::Subtraction;
    case TokenType::Mul:
      return BinaryOperation::Multiplication;
    case TokenType::Div:
      return BinaryOperation::Division;
    case TokenType::Modulo:
      return BinaryOperation::Modulo;
    case TokenType::LogicalAnd:
      return BinaryOperation::LogicalAnd;
    case TokenType::LogicalOr:
      return BinaryOperation::LogicalOr;
    case TokenType::BinaryAnd:
      return BinaryOperation::BinaryAnd;
    case TokenType::BinaryOr:
      return BinaryOperation::BinaryOr;
    case TokenType::BinaryXor:
      return BinaryOperation::BinaryXor;
    case TokenType::ShiftLeft:
      return BinaryOperation::ShiftLeft;
    case TokenType::ShiftRight:
      return BinaryOperation::ShiftRight;
    case TokenType::Greater:
      return BinaryOperation::Greater;
    case TokenType::GreaterOrEqual:
      return BinaryOperation::GreaterEq;
    case TokenType::Less:
      return BinaryOperation::Less;
    case TokenType::LessOrEqual:
      return BinaryOperation::LessEq;
    case TokenType::Equal:
      return BinaryOperation::Equal;
    case TokenType::NotEqual:
      return BinaryOperation::NotEqual;
    default:
      throw std::runtime_error("Unexpected token for binary operation!");
  }
}

TypeName Parser::typeNameFromToken(const Token& token) const
{
  auto type = token.type;
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