#include <exception>
#include <string>

#include "exceptions.h"
#include "../types/token.h"


TokeniseException::TokeniseException(Location location) {
  message = prefix + location.line.substr(location.column, 10);
  message += suffix + std::to_string(location.row) + "\n";
  message += location.line + "\n";
  message += std::string(location.column, ' ') + "^- unknown token\n";
}

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
