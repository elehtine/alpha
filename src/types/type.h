#ifndef TYPE_H
#define TYPE_H

#include <string>


enum class ValueType {
  Integer,
  Boolean,
  Unit,
};

std::string to_string(const ValueType& type);

#endif
