#include "ir.h"


IrVar::IrVar(std::string name): name(name) {}

IrVar::operator std::string() const { return name; }

std::string Instruction::format(std::vector<std::string> values) const {
  std::string result = "";
  bool comma = false;
  for (std::string value: values) {
    if (comma) result += ", ";
    comma = true;
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

LoadIntConst::operator std::string() const {
  std::string start = "LoadIntConst(";
  std::string end = ")\n";
  std::vector<std::string> values { start, std::to_string(value), destination, end };
  return format(values);
}

Copy::operator std::string() const {
  std::string start = "Copy(";
  std::string end = ")\n";
  std::vector<std::string> values { start, source, destination, end };
  return format(values);
}

Call::operator std::string() const {
  std::string start = "Call(";
  std::string end = ")\n";
  std::vector<std::string> values { start, function, format(arguments), destination, end };
  return format(values);
}

