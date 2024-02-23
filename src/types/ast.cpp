#include "ast.h"

#include "../tools/exceptions.h"
#include "../ir_generator.h"


namespace ast {

  Expression::Expression() {}
  Expression::~Expression() {}

  Literal::Literal(int value): value(value) {}

  std::string Literal::print(int level) const {
    std::string result = std::string(level * space, ' ');
    result += std::to_string(value);
    result += "\n";
    return result;
  }

  std::unique_ptr<interpretation::Interpretation> Literal::interpret() const {
    return std::make_unique<interpretation::Integer>(value);
  }

  IrVar Literal::visit(IrGenerator* generator) const {
    IrVar variable = generator->create_var();
    generator->add_instruction(std::make_unique<LoadIntConst>(value, variable));
    return variable;
  }

  type::Type Literal::check() {
    return type::Type::integer;
  }

  Identifier::Identifier(token::Token token):
    name(token.get_content()) {}

  Identifier::Identifier(std::string name): name(name) {}

  std::string Identifier::print(int level) const {
    std::string result = std::string(level * space, ' ');
    result += name;
    result += "\n";
    return result;
  }

  std::unique_ptr<interpretation::Interpretation> Identifier::interpret() const {
    return std::make_unique<interpretation::Integer>(1);
  }

  type::Type Identifier::check() {
    return type::Type::unknown;
  }

  IrVar Identifier::visit(IrGenerator* generator) const {
    return IrVar(name);
  }

  BinaryOp::BinaryOp(std::unique_ptr<Expression> left, token::Token* op,
      std::unique_ptr<Expression> right):
    left(std::move(left)), op(op), right(std::move(right)) {}

  std::string BinaryOp::print(int level) const {
    std::string result = left->print(level+1);
    result += std::string(level * space, ' ');
    result += op->get_content();
    result += "\n";
    result += right->print(level+1);
    return result;
  }

  std::unique_ptr<interpretation::Interpretation> BinaryOp::interpret() const {
    int left_value = *left->interpret();
    int right_value = *right->interpret();
    if (op->match({ token::Type::plus })) {
      return std::make_unique<interpretation::Integer>(left_value + right_value);
    }
    if (op->match({ token::Type::minus })) {
      return std::make_unique<interpretation::Integer>(left_value - right_value);
    }
    if (op->match({ token::Type::product })) {
      return std::make_unique<interpretation::Integer>(left_value * right_value);
    }
    if (op->match({ token::Type::division })) {
      return std::make_unique<interpretation::Integer>(left_value / right_value);
    }
    return std::make_unique<interpretation::Integer>(1);
  }

  type::Type BinaryOp::check() {
    if (left->check() != type::Type::integer) {
      throw TypeException("Expected integer, got " + type_to_string(left->check()));
    }
    if (right->check() != type::Type::integer) {
      throw TypeException("Expected integer, got " + type_to_string(left->check()));
    }
    return type::Type::integer;
  }

  IrVar BinaryOp::visit(IrGenerator* generator) const {
    std::vector<IrVar> arguments = {
      left->visit(generator),
      right->visit(generator),
    };
    IrVar function = IrVar(op->get_content());
    IrVar result = generator->create_var();
    generator->add_instruction(std::make_unique<Call>(function, arguments, result));
    return result;
  }

  IfThenElse::IfThenElse(std::unique_ptr<Expression> condition,
      std::unique_ptr<Expression> then_expression,
      std::unique_ptr<Expression> else_expression
      ): condition(std::move(condition)),
  then_expression(std::move(then_expression)),
  else_expression(std::move(else_expression)) {}

  std::string IfThenElse::print(int level) const {
    std::string result = condition->print(level+1);
    result += std::string(level * space, ' ');
    result += "if-then-else\n";
    result += then_expression->print(level+1);
    result += else_expression->print(level+1);
    return result;
  }

  std::unique_ptr<interpretation::Interpretation> IfThenElse::interpret(
      ) const {
    int cond = *condition->interpret();
    if (cond == 0) return else_expression->interpret();
    return then_expression->interpret();
  }

  type::Type IfThenElse::check() {
    throw TypeException("not implemented if");
  }

  IrVar IfThenElse::visit(IrGenerator* generator) const {
    std::unique_ptr<Instruction> then_label = generator->create_label();
    std::unique_ptr<Instruction> else_label = generator->create_label();
    IrVar cond = condition->visit(generator);
    generator->add_instruction(std::make_unique<CondJump>(
          cond, then_label.get(), else_label.get()));
    generator->add_instruction(std::move(then_label));
    then_expression->visit(generator);
    generator->add_instruction(std::move(else_label));
    else_expression->visit(generator);
    return IrVar("null");
  }

} /* ast */
