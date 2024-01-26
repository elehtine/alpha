#include <exception>
#include <string>

#include "exceptions.h"

ParseException::ParseException(const std::string& message): message(message) {}

const char* ParseException::what() const throw() {
  return message.c_str();
}
