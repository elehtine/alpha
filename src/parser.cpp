#include <vector>
#include <memory>

#include "tools/readwrite.h"
#include "tools/exceptions.h"
#include "types/token.h"
#include "parser.h"
#include "interpreter.h"


std::unique_ptr<ast::Expression> Parser::parse(std::vector<token::Token> tokens) {
  this->tokens = tokens;
  std::unique_ptr<ast::Expression> tree;
  try {
    tree = parse_expression();
  } catch (const ParseException& exception) {
    error = std::string(exception.what()) + "\n";
  }
  root = tree.get();
  return tree;
}

std::unique_ptr<ast::Literal> Parser::parse_literal(token::Token token) {
  return std::make_unique<ast::Literal>(token);
}

std::unique_ptr<ast::Identifier> Parser::parse_identifier(
    token::Token token) {
  return std::make_unique<ast::Identifier>(token);
}

std::unique_ptr<ast::Expression> Parser::parse_term() {
  token::Token token = peek();
  std::unique_ptr<ast::Expression> result;
  try {
    result = parse_literal(token);
  } catch (const ParseException& e) {
    try {
      result = parse_identifier(token);
    } catch (const ParseException& e) {
      throw ParseException("Excpected literal or identifier" +
          std::string(token));
    }
  }
  consume();
  return result;
}

std::unique_ptr<ast::Expression> Parser::parse_expression() {
  std::unique_ptr<ast::Expression> left = parse_term();
  size_t prev_pos = position;
  try {
    while (true) {
      prev_pos = position;
      token::Token token = consume();
      std::unique_ptr<ast::Expression> right = parse_term();
      left = std::make_unique<ast::BinaryOp>(std::move(left),
          token.parse_str(), std::move(right));
    }
  } catch (const ParseException& e) {}
  position = prev_pos;
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
