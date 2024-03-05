#include "type.h"

std::string to_string(const ValueType& type) {
  if (type == ValueType::Integer) return "INTEGER";
  if (type == ValueType::Boolean) return "BOOLEAN";
  if (type == ValueType::Unit) return "UNIT";
  return "UNKNOWN_TYPE";
}
