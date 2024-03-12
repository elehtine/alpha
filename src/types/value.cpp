#include <string>

#include "value.h"
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
