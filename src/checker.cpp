#include "checker.h"

#include "tools/exceptions.h"


namespace checker {

  bool check(Expression* tree) {
    if (tree != nullptr) tree->check();
    return true;
  }

} /* checker */
