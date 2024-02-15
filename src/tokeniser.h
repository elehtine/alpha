#ifndef TOKENISER_H
#define TOKENISER_H

#include <string>
#include <vector>
#include <regex>

#include "types/token.h"
#include "tools/exceptions.h"
#include "tools/readwrite.h"


class Tokeniser {
  public:
    Tokeniser(const std::string& source, Printer& printer);
    std::vector<token::Token*> get_tokens();

  private:
    std::string escape(const std::string& content);
    void tokenise();
    std::unique_ptr<token::Token> create_token();
    bool check(const std::regex& expression);

    std::string source;
    std::size_t position = 0;
    std::size_t line_start = 0;
    int line_number = 1;

    std::vector<std::unique_ptr<token::Token>> tokens;
};

#endif
