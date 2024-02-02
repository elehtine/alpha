#include "interpreter.h"

#include "tools/exceptions.h"


Interpreter::Interpreter(std::shared_ptr<ast::Expression> root
    ): root(root) {}

std::string Interpreter::prefix() const {
  return "Interpretation:";
}

Interpreter::operator std::string() const {
  return *root->interpret();
}
