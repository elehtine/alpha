#ifndef EXEPTIONS_H
#define EXEPTIONS_H

#include <exception>
#include <string>

class ParseException : public std::exception {
  public:
    ParseException(const std::string& message);
    const char* what() const throw() override;

  private:
    const std::string message;
};

#endif
