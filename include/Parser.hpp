#pragma once

#include <functional>
#include <list>
#include <memory>
#include <optional>

#include "AST.hpp"
#include "Tokenizer.hpp"

class Parser
{
public:
  Parser(std::istream& stream);

  std::unique_ptr<Node> parseProgram();
  std::unique_ptr<ExpressionNode> parseStringExpression();
  std::unique_ptr<ExpressionNode> parseLogicalExpression();
  std::unique_ptr<ExpressionNode> parseUnaryLogical();
  std::unique_ptr<ExpressionNode> parseComparisonExpression();
  std::unique_ptr<ExpressionNode> parseArithmeticExpression();
  std::unique_ptr<ExpressionNode> parseAddExpression();
  std::unique_ptr<ExpressionNode> parseFactor();
  std::unique_ptr<ExpressionNode> parseUnary();
  std::unique_ptr<ExpressionNode> parseTerm();
  std::unique_ptr<ExpressionNode> parseFunctionCall(std::optional<Token> identifierToken = {});
  std::unique_ptr<FunctionCallStatementNode> parseFunctionCallStatement(std::optional<Token> identifierToken = {});
  std::unique_ptr<FunctionCallStatementNode> parseLambdaCallStatement();
  std::unique_ptr<VariableDeclarationNode> parseVariableDeclaration();
  std::unique_ptr<AssignmentNode> parseAssignment(std::optional<Token> identifierToken = {});
  std::unique_ptr<StatementNode> parseReturnStatement();
  std::unique_ptr<BlockNode> parseBlock();
  std::unique_ptr<FunctionDeclarationNode> parseFunctionDeclaration();
  std::unique_ptr<LambdaNode> parseLambda();
  std::unique_ptr<ExpressionNode> parseLambdaCall(bool lParenSkipped = false);

private:
  Tokenizer tokenizer_;

  std::unique_ptr<ExpressionNode> 
  parseExpression(std::function<std::unique_ptr<ExpressionNode>()> parseOperand, 
    std::function<bool(const Token&)> operatorPredicate);

  TypeName parseType();
  std::unique_ptr<ExpressionNode> parseCallArgument();
  std::list<std::unique_ptr<ExpressionNode>> parseCallArgumentList();
  std::pair<std::string, TypeName> parseArgument();
  std::list<std::pair<std::string, TypeName>> parseArgumentList();

  void reportError(const std::string& msg) const;
  void expectToken(TokenType type, const std::string& msg);
  Token getToken(TokenType type, const std::string& msg);

  UnaryOperator unaryOperatorFromToken(const Token& token) const;
  BinaryOperator binaryOperatorFromToken(const Token& token) const;
  AssignmentOperator assignmentOperatorFromToken(const Token& token) const;
  TypeName typeNameFromToken(const Token& token) const;
};
