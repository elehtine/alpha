#ifndef CHECKER_H
#define CHECKER_H

#include "types/ast.h"


class Checker {
  public:
    Checker(Expression* tree, Printer* printer);

  private:
    Expression* tree;
    Printer* printer;
};

#endif
