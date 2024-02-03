#include <iostream>

#include "ir.h"
#include "../asm_generator.h"


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
  locals->add_location(function);
  for (const IrVar& variable: arguments) {
    locals->add_location(variable);
  }
  locals->add_location(destination);
}
