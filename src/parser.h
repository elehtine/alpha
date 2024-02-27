#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>

#include "types/token.h"
#include "types/ast.h"

class ParseException;

class Parser {
  public:
    Parser(Tokens& tokens, Printer* printer);
    std::unique_ptr<Expression> parse();

  private:
    std::unique_ptr<Expression> parse_expression_statement();
    std::unique_ptr<Expression> parse_expression();
    std::unique_ptr<Expression> parse_binary(int level);
    std::unique_ptr<Expression> parse_primary();

    std::unique_ptr<Identifier> parse_identifier();
    std::unique_ptr<Expression> parse_assignment();
    std::unique_ptr<Expression> parse_parenthesis();
    std::unique_ptr<Expression> parse_condition();
    std::unique_ptr<Expression> parse_block();
    std::unique_ptr<Arguments> parse_arguments();
    std::unique_ptr<Expression> parse_declaration();

    Tokens& tokens;
    Printer* printer;
    size_t position = 0;
};

#endif
