#include "interpreter.h"

#include "tools/exceptions.h"
#include "types/interpretation.h"


namespace  interpreter {

  std::unique_ptr<interpretation::Interpretation> interpret(ast::Expression* tree) {
    if (tree == nullptr) return std::make_unique<interpretation::Boolean>(false);
    return std::move(tree->interpret());
  }

} /* interpreter */
