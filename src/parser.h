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

  class Identifier : public Expression {
    public:
      Identifier(tokeniser::Token token);
      operator std::string() const override;

    private:
      std::string name;
  };

  class BinaryOp : public Expression {
    public:
      BinaryOp(std::unique_ptr<Literal> left, std::string op,
          std::unique_ptr<Literal> right);
      operator std::string() const override;

    private:
      std::unique_ptr<Literal> left;
      std::string op;
      std::unique_ptr<Literal> right;
  };

  class Parser {
    public:
      Parser(std::vector<tokeniser::Token> tokens);
      std::unique_ptr<Expression> parse();

    private:
      std::unique_ptr<Literal> parse_literal();
      std::unique_ptr<BinaryOp> parse_expression();

      tokeniser::Token peek();
      tokeniser::Token consume();

      std::vector<tokeniser::Token> tokens;
      size_t position;
  };

}; /* parser */

#endif
