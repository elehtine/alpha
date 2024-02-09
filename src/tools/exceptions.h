#ifndef EXEPTIONS_H
#define EXEPTIONS_H

#include <exception>
#include <string>


class CompileException : public std::exception {};

class TokeniseException : public CompileException {
  public:
    TokeniseException(const std::string& message);
    const char* what() const throw() override;

  private:
    const std::string prefix = "Unknown tokens: '";
    const std::string suffix = "'\n";
    const std::string message;
};

class ParseException : public CompileException {
  public:
    ParseException(const std::string& message);
    const char* what() const throw() override;

  private:
    const std::string message;
};

class InterpretException : public CompileException {
  public:
    InterpretException(const std::string& message);
    const char* what() const throw() override;

  private:
    const std::string message;
};

class TypeException : public CompileException {
  public:
    TypeException(const std::string& message);
    const char* what() const throw() override;

  private:
    const std::string message;
};


class IrGenerateException : public CompileException {
  public:
    IrGenerateException(const std::string& message);
    const char* what() const throw() override;

  private:
    const std::string message;
};

#endif
