#ifndef COMPILER_H
#define COMPILER_H

#include <string>

#include "tokeniser.h"
#include "parser.h"
#include "interpreter.h"

namespace compiler {

  class Compiler {
    public:
      Compiler(std::string source);

      std::string tokens(bool verbal);
      std::string tree(bool verbal);
      std::string interpret(bool verbal);

    private:
      std::string source;
      Tokeniser tokeniser;
      Parser parser;
      Interpreter interpreter;
  };

} /* compiler */

#endif
