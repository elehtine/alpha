#include <vector>
#include <memory>

#include "tools/readwrite.h"
#include "tokeniser.h"
#include "parser.h"


namespace parser {

  Expression::Expression() {}
  Expression::~Expression() {}

  Literal::Literal(tokeniser::Token token): value(token.parse_int()) {}

  Literal::operator std::string() const {
    return std::to_string(value) + "\n";
  }

  Identifier::Identifier(tokeniser::Token token):
    name(token.parse_str()) {}

  Identifier::operator std::string() const {
    return name + "\n";
  }

  BinaryOp::BinaryOp(std::unique_ptr<Expression> left, std::string op,
      std::unique_ptr<Expression> right):
    left(std::move(left)), op(op), right(std::move(right)) {}

  BinaryOp::operator std::string() const {
    return std::string(*left) + op + "\n" + std::string(*right);
  }

  Parser::Parser(std::vector<tokeniser::Token> tokens): tokens(tokens), position(0) {
    parse_lines();
  }

  std::vector<std::unique_ptr<Expression>>& Parser::get_tree() {
    return tree;
  }

  std::unique_ptr<Literal> Parser::parse_literal() {
    tokeniser::Token token = consume();
    return std::make_unique<Literal>(token);
  }

  std::unique_ptr<Identifier> Parser::parse_identifier() {
    tokeniser::Token token = consume();
    return std::make_unique<Identifier>(token);
  }

  std::unique_ptr<Expression> Parser::parse_term() {
    size_t prev_pos = position;
    try {
      return parse_literal();
    } catch (const ParseException& e) {}
    position = prev_pos;
    return parse_identifier();
  }

  std::unique_ptr<Expression> Parser::parse_expression() {
    std::unique_ptr<Expression> left = parse_term();
    size_t prev_pos = position;
    try {
      while (true) {
        prev_pos = position;
        tokeniser::Token token = consume();
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
      tokeniser::Token token = peek();
      if (token.get_type() == tokeniser::Type::eof) break;
      tree.push_back(parse_expression());
    }
  }

  tokeniser::Token Parser::peek() {
    if (position < tokens.size()) return tokens[position];
    return tokeniser::Token(tokeniser::Type::eof, "EOF");
  }

  tokeniser::Token Parser::consume() {
    tokeniser::Token token = peek();
    position++;
    return token;
  }

}; /* parser */
