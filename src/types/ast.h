#ifndef AST_H
#define AST_H

#include <string>
#include <memory>

#include "token.h"
#include "ir.h"
#include "interpretation.h"
#include "type.h"


class IrGenerator;

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
    Literal(std::string value, type::Type type);

    std::string print(int level) const override;
    std::unique_ptr<interpretation::Interpretation> interpret() const override;
    type::Type check() override;

    IrVar visit(IrGenerator* generator) const override;

  private:
    std::string value;
    type::Type type;
};

class Identifier : public Expression {
  public:
    Identifier(Token token);

    std::string print(int level) const override;
    std::unique_ptr<interpretation::Interpretation> interpret() const override;
    type::Type check() override;

    IrVar visit(IrGenerator* generator) const override;

    bool is_name(std::string guess) const;

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

class While : public Expression {
  public:
    While(std::unique_ptr<Expression> condition,
        std::unique_ptr<Expression> do_expression);
    std::string print(int level) const override;
    std::unique_ptr<interpretation::Interpretation> interpret() const override;
    type::Type check() override;
    IrVar visit(IrGenerator* generator) const override;

  private:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Expression> do_expression;
};

class Block : public Expression {
  public:
    Block(std::vector<std::unique_ptr<Expression>> expressions);
    std::string print(int level) const override;
    std::unique_ptr<interpretation::Interpretation> interpret() const override;
    type::Type check() override;

    IrVar visit(IrGenerator* generator) const override;

  private:
    std::vector<std::unique_ptr<Expression>> expressions;
};

class Arguments: public Expression {
  public:
    Arguments(std::vector<std::unique_ptr<Expression>>& arguments);

    std::string print(int level) const override;
    std::unique_ptr<interpretation::Interpretation> interpret() const override;
    type::Type check() override;
    IrVar visit(IrGenerator* generator) const override;

  private:
    std::vector<std::unique_ptr<Expression>> arguments;
};

class Function: public Expression {
  public:
    Function(std::unique_ptr<Identifier> fun, std::unique_ptr<Arguments> arguments);

    std::string print(int level) const override;
    std::unique_ptr<interpretation::Interpretation> interpret() const override;
    type::Type check() override;
    IrVar visit(IrGenerator* generator) const override;

  private:
    std::unique_ptr<Identifier> fun;
    std::unique_ptr<Arguments> arguments;
};

class Declaration: public Expression {
  public:
    Declaration(
        std::unique_ptr<Identifier> name,
        std::unique_ptr<Identifier> type,
        std::unique_ptr<Expression> value);

    std::string print(int level) const override;
    std::unique_ptr<interpretation::Interpretation> interpret() const
      override;
    type::Type check() override;
    IrVar visit(IrGenerator* generator) const override;

  private:
    std::unique_ptr<Identifier> name;
    std::unique_ptr<Identifier> type;
    std::unique_ptr<Expression> value;
};

#endif
