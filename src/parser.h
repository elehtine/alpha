#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>

#include "types/token.h"
#include "types/ast.h"
#include "interpreter.h"

#include "tools/readwrite.h"


class Parser {
  public:
    Parser(Tokens& tokens, Printer* printer);
    std::unique_ptr<ast::Expression> parse();

    std::unique_ptr<ast::Expression> parse_block();

  private:
    std::unique_ptr<ast::Expression> parse_statement();
    std::unique_ptr<ast::Expression> parse_expression();
    std::unique_ptr<ast::Expression> parse_binary(int level);
    std::unique_ptr<ast::Expression> parse_primary();

    std::unique_ptr<ast::Expression> parse_parenthesis();
    std::unique_ptr<ast::Expression> parse_condition();

    Tokens& tokens;
    Printer* printer;
    size_t position = 0;
};

#endif
