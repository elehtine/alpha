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

std::unique_ptr<Expression> Parser::parse() {
  std::vector<std::unique_ptr<Expression>> expressions;
  while (!tokens.match(token::Type::eof)) {
    expressions.push_back(parse_expression());
    if (!tokens.match(token::Type::semicolon) && !tokens.previous()->match(token::Type::right_brace)) {
      throw ParseException("Expected semicolon, got " + std::string(*tokens.peek()));
    }
  }
  if (expressions.size() == 0) {
    throw ParseException("File cannot be empty");
  }

  std::unique_ptr<Expression> root =
    std::make_unique<Block>(std::move(expressions));
  printer->print_tree(root.get());
  return root;
}

std::unique_ptr<Expression> Parser::parse_expression() {
  if (tokens.match(token::Type::keyword_if)) return parse_condition();
  if (tokens.match(token::Type::left_brace)) return parse_block();
  return parse_binary(expression);
}

std::unique_ptr<Expression> Parser::parse_parenthesis() {
  std::unique_ptr<Expression> result = parse_expression();
  if (!tokens.match(token::Type::right_parenthesis)) {
    throw ParseException(tokens.peek()->message({ token::Type::right_parenthesis }));
  }
  return result;
}

std::unique_ptr<Expression> Parser::parse_condition() {
  std::unique_ptr<Expression> condition = parse_expression();

  if (!tokens.match(token::Type::keyword_then)) {
    throw ParseException("Expected then, got "+ tokens.peek()->get_content());
  }
  std::unique_ptr<Expression> then_expression = parse_expression();

  std::unique_ptr<Expression> else_expression = std::make_unique<Literal>("null", type::Type::unit);
  if (tokens.match(token::Type::keyword_else)) {
    else_expression = parse_expression();
  }

  return std::make_unique<IfThenElse>(std::move(condition),
        std::move(then_expression), std::move(else_expression));
}

std::unique_ptr<Expression> Parser::parse_block() {
  std::vector<std::unique_ptr<Expression>> expressions;
  while (true) {
    expressions.push_back(std::move(parse_expression()));

    if (!tokens.match(token::Type::semicolon) && !tokens.previous()->match(token::Type::right_brace)) {
      if (tokens.match(token::Type::right_brace)) break;
      throw ParseException("Expected semicolon or closing brace, got " + std::string(*tokens.peek()));
    }

    if (tokens.match(token::Type::right_brace)) {
      expressions.push_back(std::make_unique<Literal>("null", type::Type::unit));
      break;
    }
  }
  return std::make_unique<Block>(std::move(expressions));
}

std::unique_ptr<Arguments> Parser::parse_arguments() {
  tokens.match(token::Type::left_parenthesis);
  std::vector<std::unique_ptr<Expression>> args;
  while (true) {
    args.push_back(parse_expression());
    if (!tokens.match(token::Type::comma)) break;
  }
  if (!tokens.match(token::Type::right_parenthesis)) {
    throw ParseException("Expected right parenthesis, got " + std::string(*tokens.peek()));
  }
  return std::make_unique<Arguments>(args);
}

std::unique_ptr<Expression> Parser::parse_binary(int level) {
  if (level == primary) return parse_primary();
  std::unique_ptr<Expression> left = parse_binary(level + 1);
  while (tokens.peek()->level() == level) {
    Token* token = tokens.consume();
    std::unique_ptr<Expression> right = parse_binary(level + 1);
    left = std::make_unique<BinaryOp>(std::move(left),
        token, std::move(right));
  }
  return left;
}

std::unique_ptr<Expression> Parser::parse_primary() {
  Token* token = tokens.peek();
  if (tokens.match(token::Type::left_parenthesis)) return parse_parenthesis();
  if (tokens.match(token::Type::keyword_true) || tokens.match(token::Type::keyword_false)) {
    return std::make_unique<Literal>(token->get_content(), type::Type::boolean);
  }
  if (tokens.match(token::Type::literal)) {
    return std::make_unique<Literal>(token->get_content(), type::Type::integer);
  }
  if (tokens.match(token::Type::identifier)) {
    std::unique_ptr<Identifier> id = std::make_unique<Identifier>(*token);
    if (!tokens.match(token::Type::left_parenthesis)) return id;
    return std::make_unique<Function>(std::move(id), parse_arguments());
  }
  throw ParseException("Expected literal or identifier, got " + std::string(*token));
}
