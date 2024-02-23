#include "type.h"

namespace type {

  std::string type_to_string(const Type& type) {
    if (type == Type::integer) return "integer";
    if (type == Type::boolean) return "boolean";
    return "unknown";
  }

} /* type */
