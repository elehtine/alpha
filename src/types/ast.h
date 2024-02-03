#ifndef AST_H
#define AST_H

#include <string>
#include <memory>

#include "token.h"
#include "interpretation.h"


namespace ast {

  class Expression {
    public:
      virtual ~Expression();
      virtual operator std::string() const = 0;
      virtual std::unique_ptr<interpretation::Interpretation> interpret() = 0;

    protected:
      Expression();
  };

  class Literal : public Expression {
    public:
      Literal(token::Token token);
      operator std::string() const override;
      virtual std::unique_ptr<interpretation::Interpretation> interpret() override;

    private:
      int interpretation;
  };

  class Identifier : public Expression {
    public:
      Identifier(token::Token token);
      operator std::string() const override;
      virtual std::unique_ptr<interpretation::Interpretation> interpret() override;

    private:
      std::string name;
  };

  class BinaryOp : public Expression {
    public:
      BinaryOp(std::unique_ptr<Expression> left, std::string op,
          std::unique_ptr<Expression> right);
      operator std::string() const override;
      virtual std::unique_ptr<interpretation::Interpretation> interpret() override;

    private:
      std::unique_ptr<Expression> left;
      std::string op;
      std::unique_ptr<Expression> right;
  };

} /* ast  */ 

#endif
