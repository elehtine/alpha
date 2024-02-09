#include <exception>
#include <string>

#include "exceptions.h"


TokeniseException::TokeniseException(const std::string& message):
  message(prefix + message + suffix) {}

const char* TokeniseException::what() const throw() {
  return message.c_str();
}

ParseException::ParseException(const std::string& message):
  message(message + "\n") {}

const char* ParseException::what() const throw() {
  return message.c_str();
}

InterpretException::InterpretException(const std::string& message):
  message(message + "\n") {}

const char* InterpretException::what() const throw() {
  return message.c_str();
}

TypeException::TypeException(const std::string& message):
  message(message + "\n") {}

const char* TypeException::what() const throw() {
  return message.c_str();
}

IrGenerateException::IrGenerateException(const std::string& message):
  message(message + "\n") {}

const char* IrGenerateException::what() const throw() {
  return message.c_str();
}
