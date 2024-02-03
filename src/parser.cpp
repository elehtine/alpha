#include <vector>
#include <memory>
#include <iostream>

#include "tools/readwrite.h"
#include "tools/exceptions.h"
#include "types/token.h"
#include "parser.h"
#include "interpreter.h"


Parser::Parser(std::vector<token::Token> tokens, bool verbose
    ): tokens(tokens), verbose(verbose) {
  try {
    root = parse_expression();
  } catch (const ParseException& e) {
    if (verbose) std::cout << e.what() << std::endl;
  }
}

ast::Expression* Parser::get_ast() {
  return root.get();
}

std::unique_ptr<ast::Literal> Parser::parse_literal(token::Token token) {
  return std::make_unique<ast::Literal>(token);
}

std::unique_ptr<ast::Identifier> Parser::parse_identifier(
    token::Token token) {
  return std::make_unique<ast::Identifier>(token);
}

std::unique_ptr<ast::Expression> Parser::parse_parenthesis(
    token::Token token) {
  consume();
  std::unique_ptr<ast::Expression> result = parse_expression();
  consume();
  return result;
}

std::unique_ptr<ast::Expression> Parser::parse_term() {
  token::Token token = peek();
  std::unique_ptr<ast::Expression> result;
  if (token.get_type() == token::Type::literal) result = parse_literal(token);
  else if (token.get_type() == token::Type::identifier) result = parse_identifier(token);
  else if (token.get_type() == token::Type::punctuation) result = parse_parenthesis(token);
  else throw ParseException("Excpected parenthesis, literal or identifier" + std::string(token));
  consume();
  return result;
}

std::unique_ptr<ast::Expression> Parser::parse_factor() {
  std::unique_ptr<ast::Expression> left = parse_term();
  while (peek().get_content() == "*" || peek().get_content() == "/") {
    token::Token token = consume();
    std::unique_ptr<ast::Expression> right = parse_term();
    left = std::make_unique<ast::BinaryOp>(std::move(left),
        token.parse_str(), std::move(right));
  }
  return left;
}

std::unique_ptr<ast::Expression> Parser::parse_expression() {
  std::unique_ptr<ast::Expression> left = parse_factor();
  while (peek().get_content() == "+" || peek().get_content() == "-") {
    token::Token token = consume();
    std::unique_ptr<ast::Expression> right = parse_factor();
    left = std::make_unique<ast::BinaryOp>(std::move(left),
        token.parse_str(), std::move(right));
  }
  return left;
}

token::Token Parser::peek() {
  if (position < tokens.size()) return tokens[position];
  return token::Token(token::Type::eof, "EOF");
}

token::Token Parser::consume() {
  token::Token token = peek();
  position++;
  return token;
}
