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
      virtual std::string print(int level) const = 0;
      virtual std::unique_ptr<interpretation::Interpretation> interpret() = 0;

    protected:
      Expression();
      const int space = 2;
  };

  class Literal : public Expression {
    public:
      Literal(token::Token token);
      std::string print(int level) const override;
      std::unique_ptr<interpretation::Interpretation> interpret() override;

    private:
      int interpretation;
  };

  class Identifier : public Expression {
    public:
      Identifier(token::Token token);
      std::string print(int level) const override;
      std::unique_ptr<interpretation::Interpretation> interpret() override;

    private:
      std::string name;
  };

  class BinaryOp : public Expression {
    public:
      BinaryOp(std::unique_ptr<Expression> left, std::string op,
          std::unique_ptr<Expression> right);
      std::string print(int level) const override;
      std::unique_ptr<interpretation::Interpretation> interpret() override;

    private:
      std::unique_ptr<Expression> left;
      std::string op;
      std::unique_ptr<Expression> right;
  };

} /* ast  */ 

#endif
