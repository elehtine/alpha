#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>

#include "types/ast.h"
#include "types/interpretation.h"


namespace  interpreter {

  std::unique_ptr<interpretation::Interpretation> interpret(ast::Expression* tree);

} /* interpreter */

#endif
