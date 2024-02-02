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
    Compiler(std::string source);
    void compile(Printer& printer);

  private:
    std::string source;
    Tokeniser tokeniser;
    Parser parser;
    Interpreter interpreter;

    std::vector<token::Token> t;
    std::unique_ptr<ast::Expression> ast;
    std::unique_ptr<value::Value> value;
};

#endif
