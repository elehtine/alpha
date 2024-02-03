#include "interpreter.h"

#include "tools/exceptions.h"
#include "types/value.h"


namespace  interpreter {

  std::unique_ptr<value::Value> interpret(ast::Expression* tree) {
    if (tree == nullptr) return std::make_unique<value::Boolean>(false);
    return std::move(tree->interpret());
  }

} /* interpreter */
