#include "ast.h"

#include "../tools/exceptions.h"


namespace ast {

  Expression::Expression() {}
  Expression::~Expression() {}

  Literal::Literal(token::Token token): value(token.parse_int()) {}

  std::string Literal::print(int level) const {
    std::string result = std::string(level * space, ' ');
    result += std::to_string(value);
    result += "\n";
    return result;
  }

  std::unique_ptr<interpretation::Interpretation> Literal::interpret() const {
    return std::make_unique<interpretation::Integer>(value);
  }

  int Literal::get_value() const {
    return value;
  }

  type::Type Literal::check() {
    return type::Type::integer;
  }

  Identifier::Identifier(token::Token token):
    name(token.parse_str()) {}

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

  BinaryOp::BinaryOp(std::unique_ptr<Expression> left, std::string op,
      std::unique_ptr<Expression> right):
    left(std::move(left)), op(op), right(std::move(right)) {}

  std::string BinaryOp::print(int level) const {
    std::string result = left->print(level+1);
    result += std::string(level * space, ' ');
    result += op;
    result += "\n";
    result += right->print(level+1);
    return result;
  }

  std::unique_ptr<interpretation::Interpretation> BinaryOp::interpret() const {
    int left_value = *left->interpret();
    int right_value = *right->interpret();
    if (op == "+") return std::make_unique<interpretation::Integer>(left_value + right_value);
    if (op == "-") return std::make_unique<interpretation::Integer>(left_value - right_value);
    if (op == "*") return std::make_unique<interpretation::Integer>(left_value * right_value);
    if (op == "/") return std::make_unique<interpretation::Integer>(left_value / right_value);
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

  Expression* BinaryOp::get_left() const {
    return left.get();
  }

  std::string BinaryOp::get_op() const {
    return op;
  }

  Expression* BinaryOp::get_right() const {
    return right.get();
  }


} /* ast */ 
