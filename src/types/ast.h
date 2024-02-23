#ifndef AST_H
#define AST_H

#include <string>
#include <memory>

#include "token.h"
#include "ir.h"
#include "interpretation.h"
#include "type.h"


class IrGenerator;

namespace ast {

  class Expression {
    public:
      virtual ~Expression();
      virtual std::string print(int level) const = 0;
      virtual std::unique_ptr<interpretation::Interpretation> interpret() const = 0;
      virtual type::Type check() = 0;

      virtual IrVar visit(IrGenerator* generator) const = 0;

    protected:
      Expression();
      const int space = 2;
  };

  class Literal : public Expression {
    public:
      Literal(int value);

      std::string print(int level) const override;
      std::unique_ptr<interpretation::Interpretation> interpret() const override;
      type::Type check() override;

      IrVar visit(IrGenerator* generator) const override;

    private:
      int value;
  };

  class Identifier : public Expression {
    public:
      Identifier(Token token);
      Identifier(std::string name);

      std::string print(int level) const override;
      std::unique_ptr<interpretation::Interpretation> interpret() const override;
      type::Type check() override;

      IrVar visit(IrGenerator* generator) const override;

    private:
      std::string name;
  };

  class BinaryOp : public Expression {
    public:
      BinaryOp(std::unique_ptr<Expression> left, Token* op,
          std::unique_ptr<Expression> right);
      std::string print(int level) const override;
      std::unique_ptr<interpretation::Interpretation> interpret() const override;
      type::Type check() override;

      IrVar visit(IrGenerator* generator) const override;

    private:
      std::unique_ptr<Expression> left;
      Token* op;
      std::unique_ptr<Expression> right;
  };

  class IfThenElse : public Expression {
    public:
      IfThenElse(std::unique_ptr<Expression> condition,
          std::unique_ptr<Expression> then_expression,
          std::unique_ptr<Expression> else_expression);
      std::string print(int level) const override;
      std::unique_ptr<interpretation::Interpretation> interpret() const override;
      type::Type check() override;

      IrVar visit(IrGenerator* generator) const override;

    private:
      std::unique_ptr<Expression> condition;
      std::unique_ptr<Expression> then_expression;
      std::unique_ptr<Expression> else_expression;
  };

  class Block : public Expression {
    public:
      Block(std::vector<std::unique_ptr<Expression>> expressions);
      std::string print(int level) const;
      std::unique_ptr<interpretation::Interpretation> interpret() const;
      type::Type check();

      IrVar visit(IrGenerator* generator) const;

    private:
      std::vector<std::unique_ptr<Expression>> expressions;
  };

} /* ast  */

#endif
