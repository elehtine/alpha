#include <string>

#include "interpretation.h"
#include "../tools/exceptions.h"


Value::Value(): type(ValueType::Unit), value(0) {}
Value::Value(ValueType type, int value): type(type), value(value) {}

Value::operator std::string() const {
  if (type == ValueType::Integer) return std::to_string(value);
  if (type == ValueType::Boolean) {
    if (value == 1) return "TRUE";
    return "FALSE";
  }
  return "UNIT";
}


SymTab::SymTab(std::unique_ptr<SymTab> parent):
  parent(std::move(parent))
{}

void SymTab::assign_variable(std::string identifier, Value value) {
  if (symbols.count(identifier)) {
    symbols[identifier] = value;
    return;
  }
  if (parent) {
    parent->assign_variable(identifier, value);
    return;
  }
  throw InterpretException("Undeclared identifier in assignment: " + identifier);
}

void SymTab::declare_variable(std::string identifier, Value value) {
  symbols[identifier] = value;
}

Value SymTab::get_variable(std::string identifier) {
  if (symbols.count(identifier)) return symbols[identifier];
  if (parent) return parent->get_variable(identifier);
  throw InterpretException("Undeclared identifier used: " + identifier);
}

std::unique_ptr<SymTab> SymTab::get_parent() {
  return std::move(parent);
}
