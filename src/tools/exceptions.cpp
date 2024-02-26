#include <exception>
#include <string>

#include "exceptions.h"
#include "../types/token.h"


CompileException::CompileException(
    const std::string type, const std::string& message):
  message(type + message + "\n")
{}

const char* CompileException::what() const throw() {
  return message.c_str();
}

TokeniseException::TokeniseException(const std::string& message):
  CompileException("TokeniseException: ", message)
{}

ParseException::ParseException(const std::string& message):
  CompileException("ParseException: ", message)
{}

InterpretException::InterpretException(const std::string& message):
  CompileException("InterpretException: ", message)
{}

TypeException::TypeException(const std::string& message):
  CompileException("TypeException: ", message)
{}

IrGenerateException::IrGenerateException(const std::string& message):
  CompileException("IrGenerateException: ", message)
{}
