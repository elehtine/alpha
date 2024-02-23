#ifndef TOKENISER_H
#define TOKENISER_H

#include <string>
#include <vector>
#include <regex>

#include "source.h"

#include "types/token.h"
#include "tools/exceptions.h"
#include "tools/readwrite.h"


class Tokens {
  public:
    Tokens(const Source& source, Printer* printer);
    std::vector<token::Token*> get_tokens();

    token::Token* peek() const;
    token::Token* consume();

  private:
    void tokenise();
    std::unique_ptr<token::Token> create_token();
    bool check(const std::regex& expression);

    const Source& source;
    Printer* printer;

    int line;
    int column;

    std::vector<std::unique_ptr<token::Token>> tokens;
    size_t position = 0;

    const std::regex whitespace { "^(\\s)+" };
    const std::regex comment { "^\\s*(//|#)[^\\\\]+" };
    const std::regex punctuation { "^(\\(|\\)|\\{|\\}|,|:|;)" };
    const std::regex oper { "^(\\+|-|\\*|/|%|=|==|!=|<|<=|>|>=)" };
    const std::regex keyword { "(var|if|then|else|while)" };
    const std::regex identifier { "^[a-zA-Z_]+" };
    const std::regex literal { "^\\d+" };
};

#endif
