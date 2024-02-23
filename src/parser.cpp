#include <vector>
#include <memory>

#include "tools/readwrite.h"
#include "tools/exceptions.h"
#include "types/token.h"
#include "parser.h"
#include "interpreter.h"


bool binary(int level) {
  return level < primary;
}
bool unary(int level) { return primary <= level; }

Parser::Parser(Tokens& tokens, Printer* printer):
  tokens(tokens), printer(printer) {}

std::unique_ptr<ast::Expression> Parser::parse_statement() {
  std::unique_ptr<ast::Expression> result = parse_expression();
  tokens.match({ token::Type::semicolon });
  return result;
}

std::unique_ptr<ast::Expression> Parser::parse_expression() {
  if (tokens.peek()->get_content() == "if") return parse_condition();
  if (tokens.match({ token::Type::left_brace})) return parse_block();
  return parse_binary(expression);
}

std::unique_ptr<ast::Expression> Parser::parse() {
  std::vector<std::unique_ptr<ast::Expression>> expressions;
  while (!tokens.match({ token::Type::eof})) {
    expressions.push_back(parse_statement());
  }
  if (expressions.size() == 0) {
    throw ParseException("File cannot be empty");
  }

  std::unique_ptr<ast::Expression> root =
    std::make_unique<ast::Block>(std::move(expressions));
  printer->print_tree(root.get());
  return root;
}

std::unique_ptr<ast::Expression> Parser::parse_parenthesis() {
  std::unique_ptr<ast::Expression> result = parse_expression();
  if (!tokens.match({ token::Type::right_parenthesis })) {
    throw ParseException(tokens.peek()->message({ token::Type::right_parenthesis }));
  }
  return result;
}

std::unique_ptr<ast::Expression> Parser::parse_condition() {
  Token* if_token = tokens.consume();
  std::unique_ptr<ast::Expression> condition = parse_expression();
  Token* then_token = tokens.consume();
  std::unique_ptr<ast::Expression> then_expression = parse_expression();
  Token* else_token = tokens.consume();
  std::unique_ptr<ast::Expression> else_expression = parse_expression();

  if (if_token->get_content() != "if") {
    throw ParseException("Expected if, got "+if_token->get_content());
  }
  if (then_token->get_content() != "then") {
    throw ParseException("Expected then, got "+then_token->get_content());
  }
  if (else_token->get_content() != "else") {
    throw ParseException("Expected else, got "+else_token->get_content());
  }
  return std::make_unique<ast::IfThenElse>(std::move(condition),
        std::move(then_expression), std::move(else_expression));
}

std::unique_ptr<ast::Expression> Parser::parse_block() {
  std::vector<std::unique_ptr<ast::Expression>> expressions;
  while (!tokens.match({ token::Type::right_brace })) {
    expressions.push_back(std::move(parse_statement()));
  }
  return std::make_unique<ast::Block>(std::move(expressions));
}

std::unique_ptr<ast::Expression> Parser::parse_binary(int level) {
  if (level == primary) return parse_primary();
  std::unique_ptr<ast::Expression> left = parse_binary(level + 1);
  while (tokens.peek()->level() == level) {
    Token* token = tokens.consume();
    std::unique_ptr<ast::Expression> right = parse_binary(level + 1);
    left = std::make_unique<ast::BinaryOp>(std::move(left),
        token, std::move(right));
  }
  return left;
}

std::unique_ptr<ast::Expression> Parser::parse_primary() {
  Token* token = tokens.peek();
  if (tokens.match({ token::Type::left_parenthesis })) return parse_parenthesis();
  if (tokens.match({ token::Type::literal })) {
    return std::make_unique<ast::Literal>(std::stoi(token->get_content()));
  }
  if (tokens.match({ token::Type::identifier })) {
      return std::make_unique<ast::Identifier>(*token);
  }
  throw ParseException("Expected literal or identifier, got " + std::string(*token));
}
