#include "ast.h"

#include "../tools/exceptions.h"
#include "../ir_generator.h"


Expression::Expression() {}
Expression::~Expression() {}

Literal::Literal(std::string value, type::Type type
    ): value(value), type(type) {}

std::string Literal::print(int level) const {
  std::string result = std::string(level * space, ' ');
  result += value;
  result += "\n";
  return result;
}

std::unique_ptr<interpretation::Interpretation> Literal::interpret() const {
  if (value == "true") return std::make_unique<interpretation::Integer>(1);
  if (value == "false" || value == "null") {
    return std::make_unique<interpretation::Integer>(0);
  }
  return std::make_unique<interpretation::Integer>(std::stoi(value));
}

IrVar Literal::visit(IrGenerator* generator) const {
  IrVar variable = generator->create_var();
  int v = 0;
  if (value == "true") v = 1;
  else if (value != "false" && value != "null") v = std::stoi(value);
  generator->add_instruction(std::make_unique<LoadIntConst>(v, variable));
  return variable;
}

type::Type Literal::check() {
  return type::Type::integer;
}

Identifier::Identifier(Token token): name(token.get_content()) {}

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
  return type::Type::unit;
}

IrVar Identifier::visit(IrGenerator* generator) const {
  return IrVar(name);
}

bool Identifier::is_name(std::string guess) const {
  return name == guess;
}

BinaryOp::BinaryOp(std::unique_ptr<Expression> left, Token* op,
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
  if (op->match(token::Type::plus)) {
    return std::make_unique<interpretation::Integer>(left_value + right_value);
  }
  if (op->match(token::Type::minus)) {
    return std::make_unique<interpretation::Integer>(left_value - right_value);
  }
  if (op->match(token::Type::product)) {
    return std::make_unique<interpretation::Integer>(left_value * right_value);
  }
  if (op->match(token::Type::division)) {
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
    std::unique_ptr<Expression> else_expression):
  condition(std::move(condition)),
  then_expression(std::move(then_expression)),
  else_expression(std::move(else_expression))
{}

std::string IfThenElse::print(int level) const {
  std::string result = std::string(level * space, ' ') + "if\n";
  result += condition->print(level+1);
  result += std::string(level * space, ' ') + "then\n";
  result += then_expression->print(level+1);
  result += std::string(level * space, ' ') + "else\n";
  result += else_expression->print(level+1);
  return result;
}

std::unique_ptr<interpretation::Interpretation> IfThenElse::interpret() const
{
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

Block::Block(std::vector<std::unique_ptr<Expression>> expressions):
  expressions(std::move(expressions))
{}

std::string Block::print(int level) const {
  std::string result = std::string(level * space, ' ') + "{\n";
  for (const std::unique_ptr<Expression>& expression: expressions) {
    result += expression->print(level + 1);
  }
  result += std::string(level * space, ' ') + "}\n";
  return result;
}

std::unique_ptr<interpretation::Interpretation> Block::interpret() const
{
  if (expressions.size() == 0) throw InterpretException("Empty file");
  return expressions[0]->interpret();
}

type::Type Block::check() {
  return expressions[0]->check();
}

IrVar Block::visit(IrGenerator* generator) const {
  return expressions[0]->visit(generator);
}

Arguments::Arguments(std::vector<std::unique_ptr<Expression>>& arguments):
  arguments(std::move(arguments))
{}

std::string Arguments::print(int level) const {
  std::string result = std::string(level * space, ' ') + "(\n";
  for (const std::unique_ptr<Expression>& expression: arguments) {
    result += expression->print(level + 1);
  }
  result += std::string(level * space, ' ') + ")\n";
  return result;
}

std::unique_ptr<interpretation::Interpretation> Arguments::interpret() const {
  if (arguments.size() == 1) return arguments[0]->interpret();
  return std::make_unique<interpretation::Integer>(1);
}

type::Type Arguments::check() {
  return type::Type::unit;
}

IrVar Arguments::visit(IrGenerator* generator) const {
  if (arguments.size() == 1) return arguments[0]->visit(generator);
  throw IrGenerateException("Arguments not implemented");
}

Function::Function(std::unique_ptr<Identifier> fun, std::unique_ptr<Arguments> arguments):
  fun(std::move(fun)), arguments(std::move(arguments))
{}

std::string Function::print(int level) const {
  std::string result = std::string(level * space, ' ') + "fun\n";
  result += fun->print(level+1);
  result += arguments->print(level + 1);
  return result;
}

std::unique_ptr<interpretation::Interpretation> Function::interpret() const {
  if (fun->is_name("print_int")) return arguments->interpret();
  throw InterpretException("Unknown function");
}

type::Type Function::check() {
  return type::Type::unit;
}

IrVar Function::visit(IrGenerator* generator) const {
  return arguments->visit(generator);
  throw IrGenerateException("Function not implemented");
}
