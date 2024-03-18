#include "ir.h"
#include "../asm_generator.h"
#include "../intrinsics.h"
#include "../tools/exceptions.h"


IrVar::IrVar(): location(0, 0, ""), name("") {}
IrVar::IrVar(Location location, std::string name):
  location(location), name(name)
{}

IrVar::operator std::string() const { return name; }

bool operator<(const IrVar& left, const IrVar& right) {
  return left.name < right.name;
}

std::string Instruction::format(std::vector<std::string> values) const {
  std::string result = "";
  for (size_t index = 0; index < values.size(); index++) {
    std::string value = values[index];
    if (index >= 2 && index < values.size()-1) result += ", ";
    result += value;
  }
  return result;
}

std::string Instruction::format(std::vector<IrVar> values) const {
  std::string result = "[";
  bool comma = false;
  for (std::string value: values) {
    if (comma) result += ", ";
    comma = true;
    result += value;
  }
  result += "]";
  return result;
}

std::string Instruction::move(std::string from, std::string to) const {
  return "movq " + from + ", " + to;
}

LoadIntConst::LoadIntConst(int value, const IrVar& destination):
  value(value), destination(destination)
{}

LoadIntConst::operator std::string() const {
  std::string start = "LoadIntConst(";
  std::string end = ")";
  std::vector<std::string> values { start, std::to_string(value), destination, end };
  return format(values);
}

void LoadIntConst::add_variables(Locals* locals) const {
  locals->add_location(destination);
}

void LoadIntConst::to_asm(AssemblyGenerator* asm_generator) const {
  asm_generator->emit(std::string("# ") + std::string(*this));

  std::string dest = asm_generator->get_location(destination);
  asm_generator->emit(move("$" + std::to_string(value), dest));
  asm_generator->emit(move(dest, register_destination));
}

Copy::Copy(IrVar source, IrVar destination): source(source), destination(destination)
{}

Copy::operator std::string() const {
  std::string start = "Copy(";
  std::string end = ")";
  std::vector<std::string> values { start, source, destination, end };
  return format(values);
}

void Copy::add_variables(Locals* locals) const {
  locals->add_location(source);
  locals->add_location(destination);
}

void Copy::to_asm(AssemblyGenerator* asm_generator) const {
  asm_generator->emit(std::string("# ") + std::string(*this));

  std::string from = asm_generator->get_location(source);
  std::string to = asm_generator->get_location(destination);
  asm_generator->emit(move(from, register_a));
  asm_generator->emit(move(register_a, to));
}

Call::Call(IrVar function, std::vector<IrVar> arguments, IrVar destination):
  function(function), arguments(arguments), destination(destination)
{}

Call::operator std::string() const {
  std::string start = "Call(";
  std::string end = ")";
  std::vector<std::string> values { start, function, format(arguments), destination, end };
  return format(values);
}

void Call::add_variables(Locals* locals) const {
  for (const IrVar& variable: arguments) {
    locals->add_location(variable);
  }
  locals->add_location(destination);
}

void Call::to_asm(AssemblyGenerator* asm_generator) const {
  asm_generator->emit(std::string("# ") + std::string(*this));

  std::vector<std::string> args;
  for (IrVar var: arguments) {
    args.push_back(asm_generator->get_location(var));
  }
  std::string result = asm_generator->get_location(destination);


  if (std::string(function) == "print_int") {
    asm_generator->emit("subq $8, " + stack_pointer);
    asm_generator->emit(move(args[0], register_destination));
    asm_generator->emit("callq print_int");
    asm_generator->emit(move(register_a, result));
    asm_generator->emit("addq $8, " + stack_pointer);
  } else if (std::string(function) == "print_bool") {
    asm_generator->emit("subq $8, " + stack_pointer);
    asm_generator->emit(move(args[0], register_destination));
    asm_generator->emit("callq print_bool");
    asm_generator->emit(move(register_a, result));
    asm_generator->emit("addq $8, " + stack_pointer);
  } else if (std::string(function) == "read_int") {
    asm_generator->emit("subq $8, " + stack_pointer);
    asm_generator->emit("callq read_int");
    asm_generator->emit(move(register_a, result));
    asm_generator->emit("addq $8, " + stack_pointer);
  } else if (args.size() == 2) {
    if (std::string(function) == "+") intrinsics::plus(args, result, asm_generator);
    if (std::string(function) == "-") intrinsics::minus(args, result, asm_generator);
    if (std::string(function) == "*") intrinsics::multiply(args, result, asm_generator);
    if (std::string(function) == "/") intrinsics::divide(args, result, asm_generator);
    if (std::string(function) == "%") intrinsics::remainder(args, result, asm_generator);

    if (std::string(function) == "==") intrinsics::equal(args, result, asm_generator);
    if (std::string(function) == "!=") intrinsics::not_equal(args, result, asm_generator);
    if (std::string(function) == "<") intrinsics::less(args, result, asm_generator);
    if (std::string(function) == "<=") intrinsics::less_or_equal(args, result, asm_generator);
    if (std::string(function) == ">") intrinsics::greater(args, result, asm_generator);
    if (std::string(function) == ">=") intrinsics::greater_or_equal(args, result, asm_generator);
  } else if (args.size() == 1) {
    if (std::string(function) == "-") intrinsics::unary_minus(args, result, asm_generator);
    if (std::string(function) == "not") intrinsics::unary_not(args, result, asm_generator);
  }
}

Label::Label(int value): value(value)
{}

Label::operator std::string() const { return ".L" + std::to_string(value); }

void Label::add_variables(Locals* locals) const {}

void Label::to_asm(AssemblyGenerator* asm_generator) const {
  asm_generator->emit(std::string(*this) + ":");
}

CondJump::CondJump(IrVar condition,
    Label* then_label,
    Label* else_label):
  condition(condition),
  then_label(then_label),
  else_label(else_label)
{}

CondJump::operator std::string() const {
  std::string result = "CondJump(";
  result += std::string(condition) + ", ";
  result += std::string(*then_label) + ", ";
  result += std::string(*else_label) + ")";
  return result;
}

void CondJump::add_variables(Locals* locals) const {}

void CondJump::to_asm(AssemblyGenerator* asm_generator) const {
  std::string cond = asm_generator->get_location(condition);
  asm_generator->emit("cmpq $0, " + cond);
  asm_generator->emit("jne " + std::string(*then_label));
  asm_generator->emit("jmp " + std::string(*else_label));
}

Jump::Jump(Label* label): label(label) {}

Jump::operator std::string() const {
  return "Jump(" + std::string(*label) + ")";
}

void Jump::add_variables(Locals* locals) const {}

void Jump::to_asm(AssemblyGenerator* asm_generator) const {
  asm_generator->emit("jmp " + std::string(*label));
}
