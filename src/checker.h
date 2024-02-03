#ifndef CHECKER_H
#define CHECKER_H

#include "types/ast.h"


namespace checker {

  bool check(ast::Expression* tree);
  
} /* checker */ 

#endif
