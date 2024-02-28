#include <string>

#include "interpretation.h"
#include "../tools/exceptions.h"


Integer::Integer(int value): value(value) {}

Integer::operator int() const { return value; }
Integer::operator bool() const {
  throw InterpretException("Expected bool, got int");
}
Integer::operator std::string() const {
  return std::to_string(value) + "\n";
}

Boolean::Boolean(bool value): value(value) {}

Boolean::operator bool() const { return value; }
Boolean::operator int() const {
  throw InterpretException("Expected int, got bool");
}
Boolean::operator std::string() const {
  return "TRUE\n";
}
