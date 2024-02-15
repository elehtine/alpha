#include "ir.h"
#include "../asm_generator.h"
#include "../intrinsics.h"


IrVar::IrVar(std::string name): name(name) {}

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

LoadIntConst::LoadIntConst(int value, const IrVar& destination):
  value(value), destination(destination) {}

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
  asm_generator->emit("movq $" + std::to_string(value) + ", " + dest);
  asm_generator->emit("movq " + dest + ", %rsi");
}

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
}

Call::Call(IrVar function, std::vector<IrVar> arguments, IrVar destination):
  function(function), arguments(arguments), destination(destination) {
}

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
  std::vector<std::string> args {
    asm_generator->get_location(arguments[0]),
    asm_generator->get_location(arguments[1]),
  };

  if (std::string(arguments[0])[0] != 'x') asm_generator->emit("movq $1, " + args[0]);
  if (std::string(arguments[1])[0] != 'x') asm_generator->emit("movq $1, " + args[1]);

  std::string result = asm_generator->get_location(destination);
  if (std::string(function) == "+") intrinsics::plus(args, result, asm_generator);
  if (std::string(function) == "-") intrinsics::minus(args, result, asm_generator);
  if (std::string(function) == "*") intrinsics::multiply(args, result, asm_generator);
  if (std::string(function) == "/") intrinsics::divide(args, result, asm_generator);
  asm_generator->emit("movq " + result + ", %rsi");
}
