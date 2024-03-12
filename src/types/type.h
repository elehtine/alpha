#ifndef TYPE_H
#define TYPE_H

#include <string>
#include <vector>
#include <memory>
#include <map>


enum class ValueType {
  Unknown,
  Unit,
  Integer,
  Boolean,
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
