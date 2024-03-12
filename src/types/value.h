#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <memory>
#include <map>

#include "type.h"

class Value {
  public:
    Value();
    Value(ValueType type, int value);
    operator std::string() const;

    ValueType type;
    int value;
};

#endif
