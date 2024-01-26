#include <vector>
#include <iostream>

#include "tokeniser.h"
#include "parser.h"


namespace parser {

  Expression::Expression() {}
  Expression::~Expression() {}

  Literal::Literal(tokeniser::Token token): value(token.parse_int()) {}

  Literal::operator std::string() const {
    return std::to_string(value);
  }

  Identifier::Identifier(tokeniser::Token token):
    name(token.parse_str()) {}

  Identifier::operator std::string() const {
    return name;
  }

  Parser::Parser(std::vector<tokeniser::Token> tokens): tokens(tokens) {}

  std::unique_ptr<Expression> Parser::parse() {
    return std::make_unique<Literal>(tokens.front());
  }

}; /* parser */
