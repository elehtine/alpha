#include "interpreter.h"

#include "tools/exceptions.h"


namespace  interpreter {

  std::unique_ptr<value::Value> interpret(ast::Expression* tree) {
    return std::move(tree->interpret());
  }

} /* interpreter */
