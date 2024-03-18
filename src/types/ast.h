#ifndef AST_H
#define AST_H

#include <string>
#include <memory>
#include <vector>

#include "location.h"
#include "type.h"


class Token;
class Value;
class Interpreter;
class Checker;
class IrGenerator;
class IrVar;
class Printer;


class Expression {
  public:
    virtual ~Expression() = default;
    virtual std::string print(int level) const = 0;
    virtual Value interpret(Interpreter* interpreter) const = 0;
    virtual ValueType check(Checker* checker) = 0;

    virtual IrVar visit(IrGenerator* generator) const = 0;

  protected:
    Expression(Location location);
    Expression(Location location, ValueType type);
    Location location;
    ValueType type = ValueType::Unknown;

    const int space = 2;
};

class Literal: public Expression {
  public:
    Literal(Location location, std::string value, ValueType type);

    std::string print(int level) const override;
    Value interpret(Interpreter* interpreter) const override;
    ValueType check(Checker* checker) override;

    IrVar visit(IrGenerator* generator) const override;

  private:
    std::string value;
};

class Identifier: public Expression {
  public:
    Identifier(Token token, Location location);

    std::string print(int level) const override;
    Value interpret(Interpreter* interpreter) const override;
    ValueType check(Checker* checker) override;

    IrVar visit(IrGenerator* generator) const override;

    bool is_name(std::string guess) const;
    Value assign(Interpreter* interpreter, Value value);
    void declare_value(Interpreter* interpreter, Value value);
    void declare_type(Checker* checker, ValueType type);
    void declare_ir(IrGenerator* generator, IrVar var);

  private:
    std::string name;
};

class BinaryOp: public Expression {
  public:
    BinaryOp(std::unique_ptr<Expression> left, Token* op,
        std::unique_ptr<Expression> right, Location location);
    std::string print(int level) const override;
    Value interpret(Interpreter* interpreter) const override;
    ValueType check(Checker* checker) override;

    IrVar visit(IrGenerator* generator) const override;

  private:
    std::unique_ptr<Expression> left;
    Token* op;
    std::unique_ptr<Expression> right;
};

class Unary: public Expression {
  public:
    Unary(std::unique_ptr<Expression> expr, Token* op, Location location);
    std::string print(int level) const override;
    Value interpret(Interpreter* interpreter) const override;
    ValueType check(Checker* checker) override;
    IrVar visit(IrGenerator* generator) const override;

  private:
    std::unique_ptr<Expression> expr;
    Token* op;
};

class Assign: public Expression {
  public:
    Assign(std::unique_ptr<Identifier> identifier,
        std::unique_ptr<Expression> value, Location location);
    std::string print(int level) const override;
    Value interpret(Interpreter* interpreter) const override;
    ValueType check(Checker* checker) override;

    IrVar visit(IrGenerator* generator) const override;

  private:
    std::unique_ptr<Identifier> identifier;
    std::unique_ptr<Expression> value;
};

class IfThenElse: public Expression {
  public:
    IfThenElse(std::unique_ptr<Expression> condition,
        std::unique_ptr<Expression> then_expression,
        std::unique_ptr<Expression> else_expression,
        Location location);
    std::string print(int level) const override;
    Value interpret(Interpreter* interpreter) const override;
    ValueType check(Checker* checker) override;

    IrVar visit(IrGenerator* generator) const override;

  private:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Expression> then_expression;
    std::unique_ptr<Expression> else_expression;
};

class While: public Expression {
  public:
    While(std::unique_ptr<Expression> condition,
        std::unique_ptr<Expression> do_expression,
        Location location);
    std::string print(int level) const override;
    Value interpret(Interpreter* interpreter) const override;
    ValueType check(Checker* checker) override;
    IrVar visit(IrGenerator* generator) const override;

  private:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Expression> do_expression;
};

class Block: public Expression {
  public:
    Block(std::vector<std::unique_ptr<Expression>> expressions, Location location);
    std::string print(int level) const override;
    Value interpret(Interpreter* interpreter) const override;
    ValueType check(Checker* checker) override;

    IrVar visit(IrGenerator* generator) const override;

  private:
    std::vector<std::unique_ptr<Expression>> expressions;
};

class Arguments: public Expression {
  public:
    Arguments(std::vector<std::unique_ptr<Expression>>& arguments,
        Location location);

    std::string print(int level) const override;
    Value interpret(Interpreter* interpreter) const override;
    ValueType check(Checker* checker) override;
    ValueType check(Checker* checker, FunType fun_type);
    IrVar visit(IrGenerator* generator) const override;

    std::vector<IrVar> get_ir(IrGenerator* generator);

  private:
    std::vector<std::unique_ptr<Expression>> arguments;
};

class Function: public Expression {
  public:
    Function(std::unique_ptr<Identifier> fun,
        std::unique_ptr<Arguments> arguments,
        Location location);

    std::string print(int level) const override;
    Value interpret(Interpreter* interpreter) const override;
    ValueType check(Checker* checker) override;
    IrVar visit(IrGenerator* generator) const override;

  private:
    std::unique_ptr<Identifier> fun;
    std::unique_ptr<Arguments> arguments;
};

class Declaration: public Expression {
  public:
    Declaration(
        std::unique_ptr<Identifier> name,
        std::unique_ptr<Identifier> type_id,
        std::unique_ptr<Expression> value,
        Location location);

    std::string print(int level) const override;
    Value interpret(Interpreter* interpreter) const override;
    ValueType check(Checker* checker) override;
    IrVar visit(IrGenerator* generator) const override;

  private:
    std::unique_ptr<Identifier> name;
    std::unique_ptr<Identifier> type_id;
    std::unique_ptr<Expression> value;
};

#endif
