#include "checker.h"

#include "tools/exceptions.h"
#include "tools/printer.h"


Checker::Checker(Expression* tree, Printer* printer): tree(tree), printer(printer) {
  tree->check();
  printer->print_check(tree);
}
