#include <vector>
#include <memory>

#include "tools/readwrite.h"
#include "tools/exceptions.h"
#include "types/token.h"
#include "parser.h"
#include "interpreter.h"


namespace parser {

  Expression::Expression() {}
  Expression::~Expression() {}

  Literal::Literal(token::Token token): value(token.parse_int()) {}

  Literal::operator std::string() const {
    return std::to_string(value) + "\n";
  }

  std::unique_ptr<interpreter::Value> Literal::interpret() {
    return std::make_unique<interpreter::Integer>(value);
  }

  Identifier::Identifier(token::Token token):
    name(token.parse_str()) {}

  Identifier::operator std::string() const {
    return name + "\n";
  }

  std::unique_ptr<interpreter::Value> Identifier::interpret() {
    return std::make_unique<interpreter::Integer>(1);
  }

  BinaryOp::BinaryOp(std::unique_ptr<Expression> left, std::string op,
      std::unique_ptr<Expression> right):
    left(std::move(left)), op(op), right(std::move(right)) {}

  BinaryOp::operator std::string() const {
    return std::string(*left) + op + "\n" + std::string(*right);
  }

  std::unique_ptr<interpreter::Value> BinaryOp::interpret() {
    int left_value = *left->interpret();
    int right_value = *right->interpret();
    if (op == "+") return std::make_unique<interpreter::Integer>(left_value + right_value);
    if (op == "-") return std::make_unique<interpreter::Integer>(left_value - right_value);
    if (op == "*") return std::make_unique<interpreter::Integer>(left_value * right_value);
    if (op == "/") return std::make_unique<interpreter::Integer>(left_value / right_value);
    return std::make_unique<interpreter::Integer>(1);
  }

  Parser::Parser(std::vector<token::Token> tokens): tokens(tokens), position(0) {
    try {
      parse_lines();
    } catch (const ParseException& exception) {
      error = std::string(exception.what()) + "\n";
    }
  }

  std::vector<std::unique_ptr<Expression>>& Parser::get_tree() {
    return tree;
  }

  std::string Parser::prefix() const {
    return "AST:\n";
  }

  Parser::operator std::string() const {
    return to_string(tree) + error;
  }

  std::unique_ptr<Literal> Parser::parse_literal(token::Token token) {
    return std::make_unique<Literal>(token);
  }

  std::unique_ptr<Identifier> Parser::parse_identifier(token::Token token) {
    return std::make_unique<Identifier>(token);
  }

  std::unique_ptr<Expression> Parser::parse_term() {
    token::Token token = peek();
    std::unique_ptr<Expression> result;
    try {
      result = parse_literal(token);
    } catch (const ParseException& e) {
      try {
        result = parse_identifier(token);
      } catch (const ParseException& e) {
        throw ParseException("Excpected literal or identifier" + std::string(token));
      }
    }
    consume();
    return result;
  }

  std::unique_ptr<Expression> Parser::parse_expression() {
    std::unique_ptr<Expression> left = parse_term();
    size_t prev_pos = position;
    try {
      while (true) {
        prev_pos = position;
        token::Token token = consume();
        std::unique_ptr<Expression> right = parse_term();
        left = std::make_unique<BinaryOp>(std::move(left),
            token.parse_str(), std::move(right));
      }
    } catch (const ParseException& e) {}
    position = prev_pos;
    return left;
  }

  void Parser::parse_lines() {
    while (true) {
      token::Token token = peek();
      if (token.get_type() == token::Type::eof) break;
      tree.push_back(parse_expression());
    }
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

}; /* parser */
