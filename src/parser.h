#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>

#include "tokeniser.h"
#include "interpreter.h"


namespace parser {

  class Expression {
    public:
      virtual ~Expression();
      virtual operator std::string() const = 0;
      virtual std::unique_ptr<interpreter::Value> interpret() = 0;

    protected:
      Expression();
  };

  class Literal : public Expression {
    public:
      Literal(tokeniser::Token token);
      operator std::string() const override;
      virtual std::unique_ptr<interpreter::Value> interpret() override;

    private:
      int value;
  };

  class Identifier : public Expression {
    public:
      Identifier(tokeniser::Token token);
      operator std::string() const override;
      virtual std::unique_ptr<interpreter::Value> interpret() override;

    private:
      std::string name;
  };

  class BinaryOp : public Expression {
    public:
      BinaryOp(std::unique_ptr<Expression> left, std::string op,
          std::unique_ptr<Expression> right);
      operator std::string() const override;
      virtual std::unique_ptr<interpreter::Value> interpret() override;

    private:
      std::unique_ptr<Expression> left;
      std::string op;
      std::unique_ptr<Expression> right;
  };

  class Parser {
    public:
      Parser(std::vector<tokeniser::Token> tokens);
      std::vector<std::unique_ptr<Expression>>& get_tree();

      operator std::string() const;

    private:
      std::unique_ptr<Literal> parse_literal(tokeniser::Token token);
      std::unique_ptr<Identifier> parse_identifier(tokeniser::Token token);
      std::unique_ptr<Expression> parse_term();
      std::unique_ptr<Expression> parse_expression();
      void parse_lines();

      tokeniser::Token peek();
      tokeniser::Token consume();

      std::vector<std::unique_ptr<Expression>> tree;

      std::vector<tokeniser::Token> tokens;
      size_t position = 0;
      std::string error = "";
  };

}; /* parser */

#endif
