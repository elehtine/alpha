#include "ast.h"
#include "type.h"
#include "ir.h"

#include "../interpreter.h"
#include "../ir_generator.h"
#include "../tools/exceptions.h"
#include "../tools/readwrite.h"


Expression::Expression(Location location): location(location) {}
Expression::~Expression() {}

Literal::Literal(Location location, std::string value, type::Type type):
  Expression(location), value(value), type(type)
{}

std::string Literal::print(int level) const {
  std::string result = std::string(level * space, ' ');
  result += value;
  result += "\n";
  return result;
}

std::unique_ptr<Interpretation> Literal::interpret(Interpreter* interpreter) const {
  if (value == "true") return std::make_unique<Integer>(1);
  if (value == "false" || value == "null") {
    return std::make_unique<Integer>(0);
  }
  return std::make_unique<Integer>(std::stoi(value));
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

Identifier::Identifier(Token token, Location location):
  Expression(location), name(token.get_content())
{}

std::string Identifier::print(int level) const {
  std::string result = std::string(level * space, ' ');
  result += name;
  result += "\n";
  return result;
}

std::unique_ptr<Interpretation> Identifier::interpret(Interpreter* interpreter) const {
  std::unique_ptr<Interpretation> result = interpreter->get_variable(name);
  if (result) return result;
  throw InterpretException("Undeclared identifier\n" + location.error_mark() + "here");
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

void Identifier::assign(Interpreter* interpreter, std::unique_ptr<Interpretation> value) {
  interpreter->assign_variable(name, std::move(value));
}

BinaryOp::BinaryOp(std::unique_ptr<Expression> left, Token* op,
    std::unique_ptr<Expression> right, Location location):
  Expression(location), left(std::move(left)), op(op), right(std::move(right))
{}

std::string BinaryOp::print(int level) const {
  std::string result = left->print(level+1);
  result += std::string(level * space, ' ');
  result += op->get_content();
  result += "\n";
  result += right->print(level+1);
  return result;
}

std::unique_ptr<Interpretation> BinaryOp::interpret(Interpreter* interpreter) const {
  int left_value = *left->interpret(interpreter);
  int right_value = *right->interpret(interpreter);
  if (op->match(TokenType::plus)) {
    return std::make_unique<Integer>(left_value + right_value);
  }
  if (op->match(TokenType::minus)) {
    return std::make_unique<Integer>(left_value - right_value);
  }
  if (op->match(TokenType::product)) {
    return std::make_unique<Integer>(left_value * right_value);
  }
  if (op->match(TokenType::division)) {
    return std::make_unique<Integer>(left_value / right_value);
  }
  if (op->match(TokenType::modulo)) {
    return std::make_unique<Integer>(left_value % right_value);
  }

  if (op->match(TokenType::equal_equal)) {
    return std::make_unique<Integer>(left_value == right_value);
  }
  if (op->match(TokenType::not_equal)) {
    return std::make_unique<Integer>(left_value != right_value);
  }

  if (op->match(TokenType::less_or_equal)) {
    return std::make_unique<Integer>(left_value <= right_value);
  }
  if (op->match(TokenType::greater_or_equal)) {
    return std::make_unique<Integer>(left_value >= right_value);
  }
  if (op->match(TokenType::less)) {
    return std::make_unique<Integer>(left_value < right_value);
  }
  if (op->match(TokenType::greater)) {
    return std::make_unique<Integer>(left_value > right_value);
  }

  if (op->match(TokenType::logical_and)) {
    return std::make_unique<Integer>(left_value && right_value);
  }
  if (op->match(TokenType::logical_or)) {
    return std::make_unique<Integer>(left_value || right_value);
  }

  return std::make_unique<Integer>(1);
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

Unary::Unary(std::unique_ptr<Expression> expr, Token* op, Location location):
  Expression(location), expr(std::move(expr)), op(op)
{}

std::string Unary::print(int level) const {
  std::string result = std::string(level * space, ' ');
  result += op->get_content();
  result += "\n";
  result += expr->print(level+1);
  return result;
}

std::unique_ptr<Interpretation> Unary::interpret(Interpreter* interpreter) const {
  return expr->interpret(interpreter);
}

type::Type Unary::check() {
  return expr->check();
}

IrVar Unary::visit(IrGenerator* generator) const {
  return expr->visit(generator);
}

Assign::Assign(std::unique_ptr<Identifier> identifier,
    std::unique_ptr<Expression> value, Location location):
  Expression(location), identifier(std::move(identifier)), value(std::move(value))
{}

std::string Assign::print(int level) const {
  std::string result = std::string(level * space, ' ') + "assign\n";
  result += identifier->print(level+1);
  result += value->print(level+1);
  return result;
}

std::unique_ptr<Interpretation> Assign::interpret(Interpreter* interpreter) const {
  identifier->interpret(interpreter);
  identifier->assign(interpreter, value->interpret(interpreter));
  return identifier->interpret(interpreter);
}

type::Type Assign::check() {
  throw TypeException("not implemented if");
}

IrVar Assign::visit(IrGenerator* generator) const {
  IrVar value_ir = value->visit(generator);
  return value_ir;
}

IfThenElse::IfThenElse(std::unique_ptr<Expression> condition,
    std::unique_ptr<Expression> then_expression,
    std::unique_ptr<Expression> else_expression,
    Location location):
  Expression(location),
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

std::unique_ptr<Interpretation> IfThenElse::interpret(Interpreter* interpreter) const {
  int cond = *condition->interpret(interpreter);
  if (cond == 0) return else_expression->interpret(interpreter);
  return then_expression->interpret(interpreter);
}

type::Type IfThenElse::check() {
  throw TypeException("not implemented if");
}

IrVar IfThenElse::visit(IrGenerator* generator) const {
  std::unique_ptr<Instruction> then_label = generator->create_label();
  std::unique_ptr<Instruction> else_label = generator->create_label();
  std::unique_ptr<Instruction> end_label = generator->create_label();
  IrVar cond = condition->visit(generator);

  generator->add_instruction(std::make_unique<CondJump>(
        cond, then_label.get(), else_label.get()));

  generator->add_instruction(std::move(then_label));
  then_expression->visit(generator);
  generator->add_instruction(std::make_unique<Jump>(end_label.get()));

  generator->add_instruction(std::move(else_label));
  else_expression->visit(generator);
  generator->add_instruction(std::move(end_label));
  return IrVar("null");
}

While::While(std::unique_ptr<Expression> condition,
    std::unique_ptr<Expression> do_expression,
    Location location):
  Expression(location),
  condition(std::move(condition)),
  do_expression(std::move(do_expression))
{}

std::string While::print(int level) const {
  std::string result = std::string(level * space, ' ') + "while\n";
  result += condition->print(level+1);
  result += std::string(level * space, ' ') + "do\n";
  result += do_expression->print(level+1);
  return result;
}

std::unique_ptr<Interpretation> While::interpret(Interpreter* interpreter) const {
  return do_expression->interpret(interpreter);
}

type::Type While::check() {
  throw TypeException("not implemented if");
}

IrVar While::visit(IrGenerator* generator) const {
  std::unique_ptr<Instruction> start_label = generator->create_label();
  std::unique_ptr<Instruction> body_label = generator->create_label();
  std::unique_ptr<Instruction> end_label = generator->create_label();

  std::unique_ptr<Jump> loop_jump = std::make_unique<Jump>(start_label.get());

  generator->add_instruction(std::move(start_label));
  IrVar cond = condition->visit(generator);
  generator->add_instruction(std::make_unique<CondJump>(
        cond, body_label.get(), end_label.get()));
  generator->add_instruction(std::move(body_label));
  do_expression->visit(generator);
  generator->add_instruction(std::move(loop_jump));
  generator->add_instruction(std::move(end_label));
  return IrVar("null");
}

Block::Block(std::vector<std::unique_ptr<Expression>> expressions,
    Location location):
  Expression(location), expressions(std::move(expressions))
{}

std::string Block::print(int level) const {
  std::string result = std::string(level * space, ' ') + "{\n";
  for (const std::unique_ptr<Expression>& expression: expressions) {
    result += expression->print(level + 1);
  }
  result += std::string(level * space, ' ') + "}\n";
  return result;
}

std::unique_ptr<Interpretation> Block::interpret(Interpreter* interpreter) const {
  interpreter->start_block();
  std::unique_ptr<Interpretation> result;
  for (const std::unique_ptr<Expression>& expr: expressions) {
    result = expr->interpret(interpreter);
  }
  interpreter->end_block();
  return result;
}

type::Type Block::check() {
  return expressions[0]->check();
}

IrVar Block::visit(IrGenerator* generator) const {
  for (const std::unique_ptr<Expression>& expr: expressions) {
    expr->visit(generator);
  }
  return IrVar("unit");
}

Arguments::Arguments(std::vector<std::unique_ptr<Expression>>& arguments,
    Location location):
  Expression(location), arguments(std::move(arguments))
{}

std::string Arguments::print(int level) const {
  std::string result = std::string(level * space, ' ') + "(\n";
  for (const std::unique_ptr<Expression>& expression: arguments) {
    result += expression->print(level + 1);
  }
  result += std::string(level * space, ' ') + ")\n";
  return result;
}

std::unique_ptr<Interpretation> Arguments::interpret(Interpreter* interpreter) const {
  if (arguments.size() == 1) return arguments[0]->interpret(interpreter);
  return std::make_unique<Integer>(1);
}

type::Type Arguments::check() {
  return type::Type::unit;
}

IrVar Arguments::visit(IrGenerator* generator) const {
  if (arguments.size() == 1) return arguments[0]->visit(generator);
  throw IrGenerateException("Arguments not implemented");
}

Function::Function(std::unique_ptr<Identifier> fun,
    std::unique_ptr<Arguments> arguments, Location location):
  Expression(location),
  fun(std::move(fun)), arguments(std::move(arguments))
{}

std::string Function::print(int level) const {
  std::string result = std::string(level * space, ' ') + "fun\n";
  result += fun->print(level+1);
  result += arguments->print(level + 1);
  return result;
}

std::unique_ptr<Interpretation> Function::interpret(Interpreter* interpreter) const {
  if (fun->is_name("print_int")) {
    interpreter->add_interpretation(std::move(arguments->interpret(interpreter)));
  }
  return std::make_unique<Integer>(1);
}

type::Type Function::check() {
  return type::Type::unit;
}

IrVar Function::visit(IrGenerator* generator) const {
  return arguments->visit(generator);
}

Declaration::Declaration(
    std::unique_ptr<Identifier> name,
    std::unique_ptr<Identifier> type,
    std::unique_ptr<Expression> value,
    Location location):
  Expression(location),
  name(std::move(name)), type(std::move(type)), value(std::move(value))
{}

std::string Declaration::print(int level) const {
  std::string result = std::string(level * space, ' ') + "var\n";
  result += name->print(level + 1);
  if (type) result += type->print(level + 1);
  result += value->print(level + 1);
  return result;
}

std::unique_ptr<Interpretation> Declaration::interpret(Interpreter* interpreter) const {
  name->assign(interpreter, std::move(value->interpret(interpreter)));
  return std::make_unique<Integer>(0);
}

type::Type Declaration::check() {
  return type::Type::unit;
}

IrVar Declaration::visit(IrGenerator* generator) const {
  return value->visit(generator);
}
