#ifndef TOKENISER_H
#define TOKENISER_H

#include <string>
#include <vector>
#include <regex>

#include "source.h"

#include "types/token.h"
#include "tools/exceptions.h"
#include "tools/readwrite.h"


class Tokeniser {
  public:
    Tokeniser(const Source& source, Printer* printer);
    std::vector<token::Token*> get_tokens();

  private:
    std::string escape(const std::string& content);
    void tokenise();
    std::unique_ptr<token::Token> create_token();
    bool check(const std::regex& expression);

    Source source;
    Printer* printer;

    int line;
    int column;

    std::vector<std::unique_ptr<token::Token>> tokens;
};

#endif
