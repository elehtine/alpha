#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>


class Interpreter {
  public:
    Interpreter();
    std::string prefix() const;
    operator std::string() const;
};

#endif
