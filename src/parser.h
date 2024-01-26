#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>

#include "tokeniser.h"


namespace parser {

  class Expression {
    public:
      virtual ~Expression();
      virtual operator std::string() const = 0;

    protected:
      Expression();
  };

  class Literal : public Expression {
    public:
      Literal(tokeniser::Token token);
      operator std::string() const override;

    private:
      int value;
  };

  std::unique_ptr<Expression> parse(std::vector<tokeniser::Token> tokens);

}; /* parser */

#endif
