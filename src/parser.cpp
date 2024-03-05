#include <vector>
#include <memory>

#include "tools/printer.h"
#include "tools/exceptions.h"
#include "types/token.h"
#include "types/type.h"
#include "parser.h"
#include "interpreter.h"


Parser::Parser(Tokens& tokens, Printer* printer):
  tokens(tokens), printer(printer)
{}

std::unique_ptr<Expression> Parser::parse() {
  std::vector<std::unique_ptr<Expression>> expressions;
  Location location = tokens.peek()->copy_location();

  while (!tokens.match(TokenType::eof)) {
    expressions.push_back(parse_expression_statement());
    if (!tokens.match(TokenType::semicolon)) {
      if (!tokens.previous()->match(TokenType::right_brace)) {
        throw tokens.error(TokenType::semicolon);
      }
    }
  }

  expressions.push_back(
      std::make_unique<Literal>(location, "null", ValueType::Unit));

  std::unique_ptr<Expression> root =
    std::make_unique<Block>(std::move(expressions), location);
  printer->print_tree(root.get());
  return root;
}

std::unique_ptr<Expression> Parser::parse_expression_statement() {
  if (tokens.match(TokenType::var)) return parse_declaration();
  return parse_expression();
}

std::unique_ptr<Expression> Parser::parse_expression() {
  if (tokens.match(TokenType::left_brace)) return parse_block();
  return parse_binary(expression);
}

std::unique_ptr<Expression> Parser::parse_parenthesis() {
  std::unique_ptr<Expression> result = parse_expression();
  if (!tokens.match(TokenType::right_parenthesis)) {
    throw tokens.error(TokenType::right_parenthesis);
  }
  return result;
}

std::unique_ptr<Expression> Parser::parse_condition() {
  Location location = tokens.peek()->copy_location();
  std::unique_ptr<Expression> condition = parse_expression();

  if (!tokens.match(TokenType::keyword_then)) {
    throw tokens.error(TokenType::keyword_then);
  }
  std::unique_ptr<Expression> then_expression = parse_expression();

  std::unique_ptr<Expression> else_expression =
    std::make_unique<Literal>(location, "null", ValueType::Unit);
  if (tokens.match(TokenType::keyword_else)) {
    else_expression = parse_expression();
  }

  return std::make_unique<IfThenElse>(std::move(condition),
      std::move(then_expression), std::move(else_expression), location);
}

std::unique_ptr<Expression> Parser::parse_loop() {
  Location location = tokens.peek()->copy_location();
  std::unique_ptr<Expression> condition = parse_expression();

  if (!tokens.match(TokenType::keyword_do)) {
    throw tokens.error(TokenType::keyword_do);
  }
  std::unique_ptr<Expression> do_expression = parse_expression();

  return std::make_unique<While>(std::move(condition),
      std::move(do_expression), location);
}

std::unique_ptr<Expression> Parser::parse_block() {
  Location location = tokens.peek()->copy_location();
  std::vector<std::unique_ptr<Expression>> expressions;

  bool semicolon = true;
  while (true) {
    if (tokens.match(TokenType::right_brace)) break;
    if (tokens.previous()->match(TokenType::right_brace)) semicolon = true;

    if (!tokens.match(TokenType::semicolon) && !semicolon) {
      throw tokens.error({ TokenType::semicolon, TokenType::right_brace });
    }

    expressions.push_back(std::move(parse_expression_statement()));
    semicolon = false;
    if (tokens.match(TokenType::semicolon)) semicolon = true;
  }


  if (semicolon) {
    expressions.push_back(std::make_unique<Literal>(location, "null", ValueType::Unit));
  }
  return std::make_unique<Block>(std::move(expressions), location);
}

std::unique_ptr<Expression> Parser::parse_binary(int level) {
  Location location = tokens.peek()->copy_location();
  if (level == unary) return parse_unary();
  if (level == assignment) return parse_assignment();

  std::unique_ptr<Expression> left = parse_binary(level + 1);
  while (tokens.peek()->level() == level) {
    Token* token = tokens.consume();
    std::unique_ptr<Expression> right = parse_binary(level + 1);
    left = std::make_unique<BinaryOp>(std::move(left), token,
        std::move(right), location);
  }
  return left;
}

std::unique_ptr<Expression> Parser::parse_unary() {
  Token* token = tokens.peek();
  if (tokens.match(TokenType::keyword_not) || tokens.match(TokenType::minus)) {
    return std::make_unique<Unary>(parse_unary(), token, token->copy_location());
  }
  return parse_primary();
}

std::unique_ptr<Expression> Parser::parse_primary() {
  Token* token = tokens.peek();
  if (tokens.match(TokenType::left_parenthesis)) return parse_parenthesis();
  if (tokens.match(TokenType::keyword_if)) return parse_condition();
  if (tokens.match(TokenType::keyword_while)) return parse_loop();

  if (tokens.match({ TokenType::keyword_true, TokenType::keyword_false })) {
    return std::make_unique<Literal>(token->copy_location(),
        token->get_content(), ValueType::Boolean);
  }
  if (tokens.match(TokenType::literal)) {
    return std::make_unique<Literal>(token->copy_location(),
        token->get_content(), ValueType::Integer);
  }
  if (tokens.match(TokenType::identifier)) {
    std::unique_ptr<Identifier> id = std::make_unique<Identifier>(*token, token->copy_location());
    if (!tokens.match(TokenType::left_parenthesis)) return id;
    return std::make_unique<Function>(std::move(id), parse_arguments(), token->copy_location());
  }
  throw tokens.error({ TokenType::literal, TokenType::identifier });
}

std::unique_ptr<Identifier> Parser::parse_identifier() {
  Token* token = tokens.peek();
  if (!tokens.match(TokenType::identifier)) {
    throw tokens.error(TokenType::identifier);
  }
  return std::make_unique<Identifier>(*token, token->copy_location());
}

std::unique_ptr<Expression> Parser::parse_assignment() {
  Token* equal = tokens.peek_second();
  if (!equal->match(TokenType::equal)) {
    return parse_binary(assignment + 1);
  }
  std::unique_ptr<Identifier> name = parse_identifier();
  equal = tokens.consume();
  std::unique_ptr<Expression> value = parse_assignment();

  return std::make_unique<Assign>(std::move(name), std::move(value),
      equal->copy_location());
}

std::unique_ptr<Arguments> Parser::parse_arguments() {
  Location location = tokens.peek()->copy_location();
  std::vector<std::unique_ptr<Expression>> args;
  if (tokens.match(TokenType::right_parenthesis)) {
    return std::make_unique<Arguments>(args, location);
  }

  while (true) {
    args.push_back(parse_expression());
    if (!tokens.match(TokenType::comma)) break;
  }
  if (!tokens.match(TokenType::right_parenthesis)) {
    throw tokens.error(TokenType::right_parenthesis);
  }
  return std::make_unique<Arguments>(args, location);
}

std::unique_ptr<Expression> Parser::parse_declaration() {
  Token* token = tokens.peek();
  if (!tokens.match(TokenType::identifier)) {
    throw tokens.error(TokenType::identifier);
  }
  std::unique_ptr<Identifier> name =
    std::make_unique<Identifier>(*token, token->copy_location());

  std::unique_ptr<Identifier> type = nullptr;
  if (tokens.match(TokenType::colon)) {
    Token* token = tokens.peek();
    if (!tokens.match(TokenType::identifier)) {
      throw tokens.error(TokenType::identifier);
    }
    type = std::make_unique<Identifier>(*token, token->copy_location());
  }

  if (!tokens.match(TokenType::equal)) {
    throw tokens.error(TokenType::equal);
  }
  std::unique_ptr<Expression> expr = parse_expression();

  return std::make_unique<Declaration>(std::move(name), std::move(type),
      std::move(expr), token->copy_location());
}
