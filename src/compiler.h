#ifndef COMPILER_H
#define COMPILER_H

#include <string>
#include <memory>

#include "tokeniser.h"
#include "parser.h"
#include "interpreter.h"
#include "tools/readwrite.h"


class Compiler {
  public:
    Compiler(std::string source, bool verbose);
    void compile(Printer& printer);

  private:
    std::string source;
    Tokeniser tokeniser;
    Parser parser;

    std::vector<token::Token> tokens;
    std::unique_ptr<ast::Expression> ast;
    std::unique_ptr<interpretation::Interpretation> interpretation;
};

#endif
