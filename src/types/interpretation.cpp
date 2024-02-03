#include <string>

#include "interpretation.h"

#include "../tools/exceptions.h"


namespace interpretation {
  
  Integer::Integer(int value): value(value) {}

  Integer::operator int() { return value; }
  Integer::operator bool() {
    throw InterpretException("Expected bool, got int");
  }
  Integer::operator std::string() {
    return std::to_string(value) + "\n";
  }

  Boolean::Boolean(bool value): value(value) {}

  Boolean::operator bool() { return value; }
  Boolean::operator int() {
    throw InterpretException("Expected int, got bool");
  }
  Boolean::operator std::string() {
    return "TRUE\n";
  }

} /* value */ 
