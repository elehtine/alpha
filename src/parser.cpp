#include <vector>

#include "tokeniser.h"
#include "parser.h"


namespace parser {

  Expression::Expression() {}
  Expression::~Expression() {}

  std::unique_ptr<Expression> parse(std::vector<tokeniser::Token> tokens) {
    return std::make_unique<Literal>(tokens.front());
  }

  Literal::Literal(tokeniser::Token token): value(token.parse_int()) {}

  Literal::operator std::string() const {
    return std::to_string(value);
  }

}; /* parser */
