#include <exception>
#include <string>

#include "exceptions.h"

ParseException::ParseException(const std::string& message): message(message.c_str()) {}

const char* ParseException::what() const throw() {
  return message;
}
