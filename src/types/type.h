#ifndef TYPE_H
#define TYPE_H

#include <string>
#include <vector>


enum class ValueType {
  Integer,
  Boolean,
  Unit,
};

std::string to_string(const ValueType& type);

class FunType {
  public:
    FunType(std::initializer_list<ValueType> parameters, ValueType result);
    ValueType check(std::vector<ValueType> arguments);

  private:
    std::vector<ValueType> parameters;
    ValueType result;
};

#endif
