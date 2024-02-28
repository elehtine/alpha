#ifndef AST_H
#define AST_H

#include <string>
#include <memory>
#include <vector>

#include "location.h"


class Token;
class Interpretation;
namespace type {
  enum class Type;
}
class Interpreter;
class IrGenerator;
class IrVar;
class Printer;


class Expression {
  public:
    virtual ~Expression();
    virtual std::string print(int level) const = 0;
    virtual std::unique_ptr<Interpretation> interpret(Interpreter* interpreter) const = 0;
    virtual type::Type check() = 0;

    virtual IrVar visit(IrGenerator* generator) const = 0;

  protected:
    Expression(Location location);
    Location location;
    const int space = 2;
};

class Literal : public Expression {
  public:
    Literal(Location location, std::string value, type::Type type);

    std::string print(int level) const override;
    std::unique_ptr<Interpretation> interpret(Interpreter* interpreter) const override;
    type::Type check() override;

    IrVar visit(IrGenerator* generator) const override;

  private:
    std::string value;
    type::Type type;
};

class Identifier : public Expression {
  public:
    Identifier(Token token, Location location);

    std::string print(int level) const override;
    std::unique_ptr<Interpretation> interpret(Interpreter* interpreter) const override;
    type::Type check() override;

    IrVar visit(IrGenerator* generator) const override;

    bool is_name(std::string guess) const;

  private:
    std::string name;
};

class BinaryOp : public Expression {
  public:
    BinaryOp(std::unique_ptr<Expression> left, Token* op,
        std::unique_ptr<Expression> right, Location location);
    std::string print(int level) const override;
    std::unique_ptr<Interpretation> interpret(Interpreter* interpreter) const override;
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
        std::unique_ptr<Expression> else_expression,
        Location location);
    std::string print(int level) const override;
    std::unique_ptr<Interpretation> interpret(Interpreter* interpreter) const override;
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
        std::unique_ptr<Expression> do_expression,
        Location location);
    std::string print(int level) const override;
    std::unique_ptr<Interpretation> interpret(Interpreter* interpreter) const override;
    type::Type check() override;
    IrVar visit(IrGenerator* generator) const override;

  private:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Expression> do_expression;
};

class Block : public Expression {
  public:
    Block(std::vector<std::unique_ptr<Expression>> expressions, Location location);
    std::string print(int level) const override;
    std::unique_ptr<Interpretation> interpret(Interpreter* interpreter) const override;
    type::Type check() override;

    IrVar visit(IrGenerator* generator) const override;

  private:
    std::vector<std::unique_ptr<Expression>> expressions;
};

class Arguments: public Expression {
  public:
    Arguments(std::vector<std::unique_ptr<Expression>>& arguments,
        Location location);

    std::string print(int level) const override;
    std::unique_ptr<Interpretation> interpret(Interpreter* interpreter) const override;
    type::Type check() override;
    IrVar visit(IrGenerator* generator) const override;

  private:
    std::vector<std::unique_ptr<Expression>> arguments;
};

class Function: public Expression {
  public:
    Function(std::unique_ptr<Identifier> fun,
        std::unique_ptr<Arguments> arguments,
        Location location);

    std::string print(int level) const override;
    std::unique_ptr<Interpretation> interpret(Interpreter* interpreter) const override;
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
        std::unique_ptr<Expression> value,
        Location location);

    std::string print(int level) const override;
    std::unique_ptr<Interpretation> interpret(Interpreter* interpreter) const override;
    type::Type check() override;
    IrVar visit(IrGenerator* generator) const override;

  private:
    std::unique_ptr<Identifier> name;
    std::unique_ptr<Identifier> type;
    std::unique_ptr<Expression> value;
};

#endif
