#include <vector>
#include <memory>

#include "tools/readwrite.h"
#include "tools/exceptions.h"
#include "types/token.h"
#include "parser.h"
#include "interpreter.h"


bool binary(int level) { return level < primary; }
bool unary(int level) { return primary <= level; }

Parser::Parser(Tokens& tokens, Printer* printer):
  tokens(tokens), printer(printer)
{}

std::unique_ptr<Expression> Parser::parse() {
  std::vector<std::unique_ptr<Expression>> expressions;
  while (!tokens.match(token::Type::eof)) {
    expressions.push_back(parse_expression_statement());
    if (!tokens.match(token::Type::semicolon)) {
      if (!tokens.previous()->match(token::Type::right_brace)) {
        throw tokens.error(token::Type::semicolon);
      }
    }
  }

  std::unique_ptr<Expression> root =
    std::make_unique<Block>(std::move(expressions));
  printer->print_tree(root.get());
  return root;
}

std::unique_ptr<Expression> Parser::parse_expression_statement() {
  if (tokens.match(token::Type::var)) return parse_declaration();
  return parse_expression();
}

std::unique_ptr<Expression> Parser::parse_expression() {
  if (tokens.match(token::Type::keyword_if)) return parse_condition();
  if (tokens.match(token::Type::left_brace)) return parse_block();
  return parse_binary(expression);
}

std::unique_ptr<Expression> Parser::parse_parenthesis() {
  std::unique_ptr<Expression> result = parse_expression();
  if (!tokens.match(token::Type::right_parenthesis)) {
    throw tokens.error(token::Type::right_parenthesis);
  }
  return result;
}

std::unique_ptr<Expression> Parser::parse_condition() {
  std::unique_ptr<Expression> condition = parse_expression();

  if (!tokens.match(token::Type::keyword_then)) {
    throw tokens.error(token::Type::keyword_then);
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
    expressions.push_back(std::move(parse_expression_statement()));

    if (!tokens.match(token::Type::semicolon) && !tokens.previous()->match(token::Type::right_brace)) {
      if (tokens.match(token::Type::right_brace)) break;
      throw tokens.error({ token::Type::semicolon, token::Type::right_brace });
    }

    if (tokens.match(token::Type::right_brace)) {
      expressions.push_back(std::make_unique<Literal>("null", type::Type::unit));
      break;
    }
  }
  return std::make_unique<Block>(std::move(expressions));
}

std::unique_ptr<Expression> Parser::parse_binary(int level) {
  if (level == primary) return parse_primary();
  if (level == assignment) return parse_assignment();

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
  if (tokens.match({ token::Type::keyword_true, token::Type::keyword_false })) {
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
  throw tokens.error({ token::Type::literal, token::Type::identifier });
}

std::unique_ptr<Identifier> Parser::parse_identifier() {
  Token* token = tokens.peek();
  if (!tokens.match(token::Type::identifier)) {
    throw tokens.error(token::Type::identifier);
  }
  return std::make_unique<Identifier>(*token);
}

std::unique_ptr<Expression> Parser::parse_assignment() {
  Token* equal = tokens.peek_second();
  if (!equal->match(token::Type::equal)) {
    return parse_binary(assignment + 1);
  }
  std::unique_ptr<Identifier> name = parse_identifier();
  equal = tokens.consume();
  std::unique_ptr<Expression> value = parse_assignment();

  return std::make_unique<BinaryOp>(std::move(name), equal, std::move(value));
}

std::unique_ptr<Arguments> Parser::parse_arguments() {
  std::vector<std::unique_ptr<Expression>> args;
  if (tokens.match(token::Type::right_parenthesis)) {
    return std::make_unique<Arguments>(args);
  }

  while (true) {
    args.push_back(parse_expression());
    if (!tokens.match(token::Type::comma)) break;
  }
  if (!tokens.match(token::Type::right_parenthesis)) {
    throw tokens.error(token::Type::right_parenthesis);
  }
  return std::make_unique<Arguments>(args);
}

std::unique_ptr<Expression> Parser::parse_declaration() {
  Token* token = tokens.peek();
  if (!tokens.match(token::Type::identifier)) {
    throw tokens.error(token::Type::identifier);
  }
  std::unique_ptr<Identifier> name = std::make_unique<Identifier>(*token);

  std::unique_ptr<Identifier> type = nullptr;
  if (tokens.match(token::Type::colon)) {
    Token* token = tokens.peek();
    if (!tokens.match(token::Type::identifier)) {
      throw tokens.error(token::Type::identifier);
    }
    type = std::make_unique<Identifier>(*token);
  }

  if (!tokens.match(token::Type::equal)) {
    throw tokens.error(token::Type::equal);
  }
  std::unique_ptr<Expression> expr = parse_expression();

  return std::make_unique<Declaration>(
      std::move(name), std::move(type), std::move(expr));
}
