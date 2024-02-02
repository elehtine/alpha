#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>

#include "types/ast.h"
#include "types/value.h"


namespace  interpreter {

  std::unique_ptr<value::Value> interpret(ast::Expression* tree);

} /* interpreter */

#endif
