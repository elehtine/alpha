#include "token.h"
#include "ast.h"
#include "type.h"
#include "ir.h"

#include "../interpreter.h"
#include "../checker.h"
#include "../ir_generator.h"
#include "../tools/exceptions.h"


Expression::Expression(Location location): location(location) {}
Expression::Expression(Location location, ValueType type):
  location(location), type(type)
{}

Literal::Literal(Location location, std::string value, ValueType type):
  Expression(location, type), value(value)
{}

std::string Literal::print(int level) const {
  std::string result = std::string(level * space, ' ');
  result += value;
  if (type != ValueType::Unknown) result += " (" + to_string(type) + ")";
  result += "\n";
  return result;
}

Value Literal::interpret(Interpreter* interpreter) const {
  if (value == "null") return Value();
  if (value == "true") return Value(ValueType::Boolean, true);
  if (value == "false") return Value(ValueType::Boolean, false);
  return Value(ValueType::Integer, std::stoi(value));
}

ValueType Literal::check(Checker* checker) {
  return type;
}

IrVar Literal::visit(IrGenerator* generator) const {
  IrVar variable = generator->create_var(location);
  int v = 0;
  if (value == "true") v = 1;
  else if (value == "false" || value == "null") v = 0;
  else v = std::stoi(value);
  generator->add_instruction(std::make_unique<LoadIntConst>(v, variable));
  return variable;
}

Identifier::Identifier(Token token, Location location):
  Expression(location), name(token.get_content())
{}

std::string Identifier::print(int level) const {
  std::string result = std::string(level * space, ' ');
  result += name;
  if (type != ValueType::Unknown) result += " (" + to_string(type) + ")";
  result += "\n";
  return result;
}

Value Identifier::interpret(Interpreter* interpreter) const {
  return interpreter->get_variable(name);
}

ValueType Identifier::check(Checker* checker) {
  type = checker->get_variable(name);
  return type;
}

IrVar Identifier::visit(IrGenerator* generator) const {
  return generator->get_variable(name);
}

bool Identifier::is_name(std::string guess) const {
  return name == guess;
}

Value Identifier::assign(Interpreter* interpreter, Value value) {
  interpreter->assign_variable(name, value);
  return interpreter->get_variable(name);
}

void Identifier::declare_value(Interpreter* interpreter, Value value) {
  try {
    interpreter->declare_variable(name, value);
  } catch (const SymTabException& exception) {
    throw InterpretException(location.error("Already defined identifier"));
  }
}

void Identifier::declare_type(Checker* checker, ValueType value_type) {
  if (value_type == ValueType::Unknown || value_type == ValueType::Unit) {
    throw TypeException(location.error("Value should be Boolean or Int"));
  }

  try {
    checker->declare_variable(name, value_type);
    type = value_type;
  } catch (const SymTabException& exception) {
    throw TypeException(location.error("Already defined identifier"));
  }
}

void Identifier::declare_ir(IrGenerator* generator, IrVar var) {
  try {
    generator->declare_variable(name, var);
  } catch (const SymTabException& exception) {
    throw IrGenerateException(location.error("Already defined identifier"));
  }
}

BinaryOp::BinaryOp(std::unique_ptr<Expression> left, Token* op,
    std::unique_ptr<Expression> right, Location location):
  Expression(location), left(std::move(left)), op(op), right(std::move(right))
{}

std::string BinaryOp::print(int level) const {
  std::string result = left->print(level+1);
  result += std::string(level * space, ' ');
  result += op->get_content();
  if (type != ValueType::Unknown) result += " (" + to_string(type) + ")";
  result += "\n";
  result += right->print(level+1);
  return result;
}

Value BinaryOp::interpret(Interpreter* interpreter) const {
  if (op->match(TokenType::logical_and)) {
    Value left_value = left->interpret(interpreter);
    if (left_value.type != ValueType::Boolean && !left_value.value) return left_value;
    return right->interpret(interpreter);
  }
  if (op->match(TokenType::logical_or)) {
    Value left_value = left->interpret(interpreter);
    if (left_value.type != ValueType::Boolean && !left_value.value) return right->interpret(interpreter);
    return left_value;
  }

  Value left_value = left->interpret(interpreter);
  Value right_value = right->interpret(interpreter);
  if (left_value.type == ValueType::Integer && right_value.type == ValueType::Integer) {
    if (op->match(TokenType::plus)) {
      return Value(ValueType::Integer, left_value.value + right_value.value);
    }
    if (op->match(TokenType::minus)) {
      return Value(ValueType::Integer, left_value.value - right_value.value);
    }
    if (op->match(TokenType::product)) {
      return Value(ValueType::Integer, left_value.value * right_value.value);
    }
    if (op->match(TokenType::division)) {
      return Value(ValueType::Integer, left_value.value / right_value.value);
    }
    if (op->match(TokenType::modulo)) {
      return Value(ValueType::Integer, left_value.value % right_value.value);
    }

    if (op->match(TokenType::equal_equal)) {
      return Value(ValueType::Boolean, left_value.value == right_value.value);
    }
    if (op->match(TokenType::not_equal)) {
      return Value(ValueType::Boolean, left_value.value != right_value.value);
    }

    if (op->match(TokenType::less_or_equal)) {
      return Value(ValueType::Boolean, left_value.value <= right_value.value);
    }
    if (op->match(TokenType::greater_or_equal)) {
      return Value(ValueType::Boolean, left_value.value >= right_value.value);
    }
    if (op->match(TokenType::less)) {
      return Value(ValueType::Boolean, left_value.value < right_value.value);
    }
    if (op->match(TokenType::greater)) {
      return Value(ValueType::Boolean, left_value.value > right_value.value);
    }
  }

  throw InterpretException(location.error("Invalid operators"));
}

ValueType BinaryOp::check(Checker* checker) {
  ValueType left_type = left->check(checker);
  ValueType right_type = right->check(checker);
  FunType fun_type = op->get_funtype();

  try {
    type = fun_type.check({ left_type, right_type });
  } catch (const FunTypeException& exception) {
    throw TypeException(location.error(exception.what()));
  }
  return type;
}

IrVar BinaryOp::visit(IrGenerator* generator) const {
  std::vector<IrVar> arguments = {
    left->visit(generator),
    right->visit(generator),
  };
  IrVar function = IrVar(location, op->get_content());
  IrVar result = generator->create_var(location);
  generator->add_instruction(std::make_unique<Call>(function, arguments, result));
  return result;
}

Unary::Unary(std::unique_ptr<Expression> expr, Token* op, Location location):
  Expression(location), expr(std::move(expr)), op(op)
{}

std::string Unary::print(int level) const {
  std::string result = std::string(level * space, ' ');
  result += op->get_content();
  if (type != ValueType::Unknown) result += " (" + to_string(type) + ")";
  result += "\n";
  result += expr->print(level+1);
  return result;
}

Value Unary::interpret(Interpreter* interpreter) const {
  Value value = expr->interpret(interpreter);
  if (op->match(TokenType::keyword_not) && value.type == ValueType::Boolean) {
    return Value(ValueType::Boolean, !value.value);
  }
  if (op->match(TokenType::minus) && value.type == ValueType::Integer) {
    return Value(ValueType::Integer, -value.value);
  }

  throw InterpretException(location.error("Invalid value"));
}

ValueType Unary::check(Checker* checker) {
  type = expr->check(checker);
  if (op->match(TokenType::minus)) {
    if (type == ValueType::Integer) return type;
    throw TypeException(location.error("Expected integer, got " + to_string(type)));
  }
  if (op->match(TokenType::keyword_not)) {
    if (type == ValueType::Boolean) return type;
    throw TypeException(location.error("Expected boolean, got " + to_string(type)));
  }

  throw TypeException(location.error("Unknown unary operator " + to_string(type)));
}

IrVar Unary::visit(IrGenerator* generator) const {
  return expr->visit(generator);
}

Assign::Assign(std::unique_ptr<Identifier> identifier,
    std::unique_ptr<Expression> value, Location location):
  Expression(location), identifier(std::move(identifier)), value(std::move(value))
{}

std::string Assign::print(int level) const {
  std::string result = std::string(level * space, ' ') + "assign";
  if (type != ValueType::Unknown) result += " (" + to_string(type) + ")";
  result += "\n";
  result += identifier->print(level+1);
  result += value->print(level+1);
  return result;
}

Value Assign::interpret(Interpreter* interpreter) const {
  try {
    return identifier->assign(interpreter, value->interpret(interpreter));
  } catch (const InterpretException& e) {
    throw InterpretException(location.error("Invalid assignment"));
  }
}

ValueType Assign::check(Checker* checker) {
  type = value->check(checker);
  return type;
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
  std::string result = std::string(level * space, ' ') + "if";
  if (type != ValueType::Unknown) result += " (" + to_string(type) + ")";
  result += "\n";
  result += condition->print(level+1);
  result += std::string(level * space, ' ') + "then\n";
  result += then_expression->print(level+1);
  if (else_expression) {
    result += std::string(level * space, ' ') + "else\n";
    result += else_expression->print(level+1);
  }
  return result;
}

Value IfThenElse::interpret(Interpreter* interpreter) const {
  Value cond = condition->interpret(interpreter);
  if (cond.type != ValueType::Boolean) {
    throw InterpretException(location.error("Condition is not boolean"));
  }

  if (cond.value == true) {
    Value result = then_expression->interpret(interpreter);
    if (else_expression) return result;
    return Value();
  }

  if (else_expression) return else_expression->interpret(interpreter);
  return Value();
}

ValueType IfThenElse::check(Checker* checker) {
  ValueType cond = condition->check(checker);
  if (cond != ValueType::Boolean) {
    throw TypeException(location.error("Expected boolean, got " + to_string(cond)));
  }

  if (else_expression) {
    type = then_expression->check(checker);
    if (type != else_expression->check(checker)) {
      throw TypeException(location.error("Branches different types"));
    }
  } else {
    type = ValueType::Unit;
  }

  return type;
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
  if (else_expression) {
    else_expression->visit(generator);
  }
  generator->add_instruction(std::move(end_label));
  return IrVar(location, "null");
}

While::While(std::unique_ptr<Expression> condition,
    std::unique_ptr<Expression> do_expression,
    Location location):
  Expression(location, ValueType::Unit),
  condition(std::move(condition)),
  do_expression(std::move(do_expression))
{}

std::string While::print(int level) const {
  std::string result = std::string(level * space, ' ') + "while";
  if (type != ValueType::Unknown) result += " (" + to_string(type) + ")";
  result += "\n";
  result += condition->print(level+1);
  result += std::string(level * space, ' ') + "do\n";
  result += do_expression->print(level+1);
  return result;
}

Value While::interpret(Interpreter* interpreter) const {
  for (int time = 1; time < 10; time++) {
    if (!condition->interpret(interpreter).value) break;
    do_expression->interpret(interpreter);
  }
  return Value();
}

ValueType While::check(Checker* checker) {
  ValueType cond = condition->check(checker);
  if (cond != ValueType::Boolean) {
    throw TypeException(location.error("Expected boolean, got " + to_string(cond)));
  }

  do_expression->check(checker);
  return type;
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
  return IrVar(location, "null");
}

Block::Block(std::vector<std::unique_ptr<Expression>> expressions,
    Location location):
  Expression(location), expressions(std::move(expressions))
{}

std::string Block::print(int level) const {
  std::string result = std::string(level * space, ' ') + "{";
  if (type != ValueType::Unknown) result += " (" + to_string(type) + ")";
  result += "\n";
  for (const std::unique_ptr<Expression>& expression: expressions) {
    result += expression->print(level + 1);
  }
  result += std::string(level * space, ' ') + "}\n";
  return result;
}

Value Block::interpret(Interpreter* interpreter) const {
  interpreter->start_block();
  Value result;
  for (const std::unique_ptr<Expression>& expr: expressions) {
    result = expr->interpret(interpreter);
  }
  interpreter->end_block();
  return result;
}

ValueType Block::check(Checker* checker) {
  checker->start_block();
  ValueType result;
  for (const std::unique_ptr<Expression>& expr: expressions) {
    result = expr->check(checker);
  }
  type = result;
  checker->end_block();
  return result;
}

IrVar Block::visit(IrGenerator* generator) const {
  generator->start_block();
  for (const std::unique_ptr<Expression>& expr: expressions) {
    expr->visit(generator);
  }
  generator->end_block();
  return IrVar(location, "unit");
}

Arguments::Arguments(std::vector<std::unique_ptr<Expression>>& arguments,
    Location location):
  Expression(location), arguments(std::move(arguments))
{}

std::string Arguments::print(int level) const {
  std::string result = std::string(level * space, ' ') + "(";
  if (type != ValueType::Unknown) result += " (" + to_string(type) + ")";
  result += "\n";
  for (const std::unique_ptr<Expression>& expression: arguments) {
    result += expression->print(level + 1);
  }
  result += std::string(level * space, ' ') + ")\n";
  return result;
}

Value Arguments::interpret(Interpreter* interpreter) const {
  if (arguments.size() == 1) return arguments[0]->interpret(interpreter);
  return Value();
}

ValueType Arguments::check(Checker* checker) {
  return ValueType::Unit;
}

ValueType Arguments::check(Checker* checker, FunType fun_type) {
  std::vector<ValueType> types;
  for (const std::unique_ptr<Expression>& argument: arguments) {
    types.push_back(argument->check(checker));
  }

  ValueType result = ValueType::Unknown;
  try {
    result = fun_type.check(types);
  } catch (const FunTypeException& exception) {
    throw TypeException(location.error(exception.what()));
  }
  return result;
}

IrVar Arguments::visit(IrGenerator* generator) const {
  if (arguments.size() == 1) return arguments[0]->visit(generator);
  throw IrGenerateException(location.error("Arguments not implemented"));
}

std::vector<IrVar> Arguments::get_ir(IrGenerator* generator) {
  std::vector<IrVar> result;
  for (const std::unique_ptr<Expression>& expr: arguments) {
    result.push_back(expr->visit(generator));
  }
  return result;
}

Function::Function(std::unique_ptr<Identifier> fun,
    std::unique_ptr<Arguments> arguments, Location location):
  Expression(location),
  fun(std::move(fun)), arguments(std::move(arguments))
{}

std::string Function::print(int level) const {
  std::string result = std::string(level * space, ' ') + "fun";
  if (type != ValueType::Unknown) result += " (" + to_string(type) + ")";
  result += "\n";
  result += fun->print(level+1);
  result += arguments->print(level + 1);
  return result;
}

Value Function::interpret(Interpreter* interpreter) const {
  if (fun->is_name("print_int") || fun->is_name("print_bool")) {
    interpreter->add_interpretation(arguments->interpret(interpreter));
  }
  if (fun->is_name("read_int")) return Value(ValueType::Integer, 10);
  return Value();
}

ValueType Function::check(Checker* checker) {
  if (fun->is_name("print_int")) {
    FunType fun_type({ ValueType::Integer }, ValueType::Unit);
    type = arguments->check(checker, fun_type);
  }
  if (fun->is_name("print_bool")) {
    FunType fun_type({ ValueType::Boolean }, ValueType::Unit);
    type = arguments->check(checker, fun_type);
  }
  if (fun->is_name("read_int")) {
    FunType fun_type({}, ValueType::Integer);
    type = arguments->check(checker, fun_type);
  }
  return type;
}

IrVar Function::visit(IrGenerator* generator) const {
  if (fun->is_name("print_int")) {
    std::vector<IrVar> args = arguments->get_ir(generator);
    IrVar function = IrVar(location, "print_int");
    IrVar result = generator->create_var(location);
    generator->add_instruction(std::make_unique<Call>(function, args, result));
    return result;
  }
  if (fun->is_name("print_bool")) {
    std::vector<IrVar> args = arguments->get_ir(generator);
    IrVar function = IrVar(location, "print_bool");
    IrVar result = generator->create_var(location);
    generator->add_instruction(std::make_unique<Call>(function, args, result));
    return result;
  }
  if (fun->is_name("read_int")) {
    IrVar function = IrVar(location, "read_int");
    IrVar result = generator->create_var(location);
    generator->add_instruction(std::make_unique<Call>(function, std::vector<IrVar>{}, result));
    return result;
  }
  return IrVar(location, "null");
}

Declaration::Declaration(
    std::unique_ptr<Identifier> name,
    std::unique_ptr<Identifier> type_id,
    std::unique_ptr<Expression> value,
    Location location):
  Expression(location),
  name(std::move(name)), type_id(std::move(type_id)), value(std::move(value))
{}

std::string Declaration::print(int level) const {
  std::string result = std::string(level * space, ' ') + "var";
  if (type != ValueType::Unknown) result += " (" + to_string(type) + ")";
  result += "\n";
  result += name->print(level + 1);
  if (type_id) result += type_id->print(level + 1);
  result += value->print(level + 1);
  return result;
}

Value Declaration::interpret(Interpreter* interpreter) const {
  name->declare_value(interpreter, value->interpret(interpreter));
  return Value();
}

ValueType Declaration::check(Checker* checker) {
  ValueType t = value->check(checker);
  name->declare_type(checker, t);
  if (type_id) {
    if (t == ValueType::Boolean && !type_id->is_name("Boolean")) {
      throw TypeException(location.error("Expected value to be Int"));
    }
    if (t == ValueType::Integer && !type_id->is_name("Int")) {
      throw TypeException(location.error("Expected value to be Boolean"));
    }
  }
  return ValueType::Unit;
}

IrVar Declaration::visit(IrGenerator* generator) const {
  IrVar id = generator->create_var(location);
  name->declare_ir(generator, id);

  IrVar v = value->visit(generator);
  generator->add_instruction(std::make_unique<Copy>(v, id));
  return IrVar(location, "null");
}
