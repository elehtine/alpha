#include "ast.h"


namespace ast {

  Expression::Expression() {}
  Expression::~Expression() {}

  Literal::Literal(token::Token token): interpretation(token.parse_int()) {}

  Literal::operator std::string() const {
    return std::to_string(interpretation) + "\n";
  }

  std::unique_ptr<interpretation::Interpretation> Literal::interpret() {
    return std::make_unique<interpretation::Integer>(interpretation);
  }

  Identifier::Identifier(token::Token token):
    name(token.parse_str()) {}

  Identifier::operator std::string() const {
    return name + "\n";
  }

  std::unique_ptr<interpretation::Interpretation> Identifier::interpret() {
    return std::make_unique<interpretation::Integer>(1);
  }

  BinaryOp::BinaryOp(std::unique_ptr<Expression> left, std::string op,
      std::unique_ptr<Expression> right):
    left(std::move(left)), op(op), right(std::move(right)) {}

  BinaryOp::operator std::string() const {
    return std::string(*left) + op + "\n" + std::string(*right);
  }

  std::unique_ptr<interpretation::Interpretation> BinaryOp::interpret() {
    int left_value = *left->interpret();
    int right_value = *right->interpret();
    if (op == "+") return std::make_unique<interpretation::Integer>(left_value + right_value);
    if (op == "-") return std::make_unique<interpretation::Integer>(left_value - right_value);
    if (op == "*") return std::make_unique<interpretation::Integer>(left_value * right_value);
    if (op == "/") return std::make_unique<interpretation::Integer>(left_value / right_value);
    return std::make_unique<interpretation::Integer>(1);
  }
  
} /* ast */ 
