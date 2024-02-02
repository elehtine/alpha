#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>

#include "types/ast.h"
#include "types/value.h"


class Interpreter {
  public:
    Interpreter(std::shared_ptr<ast::Expression> root);
    std::string prefix() const;
    operator std::string() const;

  private:
    std::shared_ptr<ast::Expression> root;
};

#endif
