#include "value.h"

#include "../tools/exceptions.h"


namespace value {
  
  Integer::Integer(int value): value(value) {}

  Integer::operator int() { return value; }
  Integer::operator bool() {
    throw InterpretException("Expected bool, got int");
  }

  Boolean::Boolean(bool value): value(value) {}

  Boolean::operator bool() { return value; }
  Boolean::operator int() {
    throw InterpretException("Expected int, got bool");
  }

} /* value */ 
