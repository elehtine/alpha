#ifndef EXEPTIONS_H
#define EXEPTIONS_H

#include <exception>
#include <string>


class CompileException : public std::exception {
  public:
    CompileException(const std::string type, const std::string& message);
    const char* what() const throw() override;

  private:
    const std::string message;
};

class TokeniseException : public CompileException {
  public:
    TokeniseException(const std::string& message);

  private:
    const std::string type = "TokeniseException: ";
};

class ParseException : public CompileException {
  public:
    ParseException(const std::string& message);
};

class InterpretException : public CompileException {
  public:
    InterpretException(const std::string& message);
};

class TypeException : public CompileException {
  public:
    TypeException(const std::string& message);
};

class IrGenerateException : public CompileException {
  public:
    IrGenerateException(const std::string& message);
};

class SymTabException: public std::exception {};

class FunTypeException : public std::exception {
  public:
    FunTypeException(const std::string& message);
    const char* what() const throw() override;

  private:
    std::string message;
};

#endif
