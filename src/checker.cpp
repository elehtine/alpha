#include "checker.h"

#include "tools/exceptions.h"


Checker::Checker(Expression* tree, Printer* printer): tree(tree), printer(printer) {
  tree->check();
}
