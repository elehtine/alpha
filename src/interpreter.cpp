#include "interpreter.h"

#include "tools/exceptions.h"


Interpreter::Interpreter() {}

std::string Interpreter::prefix() const {
  return "Interpretation:";
}

Interpreter::operator std::string() const {
  return "";
}
