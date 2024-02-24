#ifndef TYPE_H
#define TYPE_H

#include <string>


namespace type {

  enum class Type {
    integer,
    boolean,
    unit,
  };

  std::string type_to_string(const Type& type);

} /* type  */

#endif
