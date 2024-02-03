#include "checker.h"

#include "tools/exceptions.h"


namespace checker {

  bool check(ast::Expression* tree) {
    bool pass = true;
    if (tree != nullptr) {
      try {
        tree->check();
      } catch (const TypeException& exception) {
        pass = false;
      }
    }
    return pass;
  }
  
} /* checker */ 
