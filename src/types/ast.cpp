#include "ast.h"


namespace ast {

  Expression::Expression() {}
  Expression::~Expression() {}

  Literal::Literal(token::Token token): value(token.parse_int()) {}

  Literal::operator std::string() const {
    return std::to_string(value) + "\n";
  }

  std::unique_ptr<value::Value> Literal::interpret() {
    return std::make_unique<value::Integer>(value);
  }

  Identifier::Identifier(token::Token token):
    name(token.parse_str()) {}

  Identifier::operator std::string() const {
    return name + "\n";
  }

  std::unique_ptr<value::Value> Identifier::interpret() {
    return std::make_unique<value::Integer>(1);
  }

  BinaryOp::BinaryOp(std::unique_ptr<Expression> left, std::string op,
      std::unique_ptr<Expression> right):
    left(std::move(left)), op(op), right(std::move(right)) {}

  BinaryOp::operator std::string() const {
    return std::string(*left) + op + "\n" + std::string(*right);
  }

  std::unique_ptr<value::Value> BinaryOp::interpret() {
    int left_value = *left->interpret();
    int right_value = *right->interpret();
    if (op == "+") return std::make_unique<value::Integer>(left_value + right_value);
    if (op == "-") return std::make_unique<value::Integer>(left_value - right_value);
    if (op == "*") return std::make_unique<value::Integer>(left_value * right_value);
    if (op == "/") return std::make_unique<value::Integer>(left_value / right_value);
    return std::make_unique<value::Integer>(1);
  }
  
} /* ast */ 
