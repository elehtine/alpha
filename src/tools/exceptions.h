#ifndef EXEPTIONS_H
#define EXEPTIONS_H

#include <exception>
#include <string>


class TokeniseException : public std::exception {
  public:
    TokeniseException(const std::string& message);
    const char* what() const throw() override;

  private:
    const std::string prefix = "Unknown tokens: '";
    const std::string suffix = "'";
    const std::string message;
};

class ParseException : public std::exception {
  public:
    ParseException(const std::string& message);
    const char* what() const throw() override;

  private:
    const std::string message;
};

class InterpretException : public std::exception {
  public:
    InterpretException(const std::string& message);
    const char* what() const throw() override;

  private:
    const std::string message;
};

class TypeException : public std::exception {
  public:
    TypeException(const std::string& message);
    const char* what() const throw() override;

  private:
    const std::string message;
};


class IrGenerateException : public std::exception {
  public:
    IrGenerateException(const std::string& message);
    const char* what() const throw() override;

  private:
    const std::string message;
};

#endif
