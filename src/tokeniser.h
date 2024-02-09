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
    std::vector<token::Token> get_tokens();

  private:
    void tokenise();
    bool check(const std::regex& expression, const token::Type& type);

    std::vector<token::Token> tokens;

    std::string source;
    std::size_t position;

    const std::vector<std::pair<std::regex, token::Type>> types = {
      { std::regex("^\\s+"), token::Type::whitespace },
      { std::regex("^(\\(|\\))"), token::Type::punctuation },
      { std::regex("^(\\+|-|\\*|/)"), token::Type::oper },
      { std::regex("^[a-zA-Z]\\w*"), token::Type::identifier },
      { std::regex("^\\d+"), token::Type::literal },
    };
};

#endif
