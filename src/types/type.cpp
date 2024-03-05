#include "type.h"
#include "../tools/exceptions.h"

std::string to_string(const ValueType& type) {
  if (type == ValueType::Integer) return "INTEGER";
  if (type == ValueType::Boolean) return "BOOLEAN";
  if (type == ValueType::Unit) return "UNIT";
  return "UNKNOWN_TYPE";
}

FunType::FunType(std::initializer_list<ValueType> parameters, ValueType result):
  parameters(parameters), result(result)
{}

ValueType FunType::check(std::vector<ValueType> arguments) {
  if (arguments.size() != parameters.size()) {
    throw TypeException("Different sized type lists");
  }

  for (std::size_t index = 0; index < arguments.size(); index++) {
    if (arguments[index] != parameters[index]) {
      throw TypeException("Invalid argument type " + to_string(arguments[index]));
    }
  }

  return result;
}
