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
    bool match(std::vector<token::Type> types);

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
    const std::regex keyword { "(var|if|then|else|while|do)" };
    const std::regex identifier { "^[a-zA-Z_]+" };
    const std::regex literal { "^\\d+" };


    std::map<std::string, token::Type> str_to_type {
      { "(", token::Type::left_parenthesis },
      { ")", token::Type::right_parenthesis },
      { "{", token::Type::left_brace },
      { "},", token::Type::right_brace },
      { ".", token::Type::dot },
      { ",", token::Type::comma },
      { ":", token::Type::colon },
      { ";", token::Type::semicolon },
      { "+", token::Type::plus },
      { "-", token::Type::minus },
      { "*", token::Type::product },
      { "/", token::Type::division },
      { "%", token::Type::modulo },
      { "!", token::Type::bang },
      { "=", token::Type::equal },
      { "==", token::Type::equal },
      { "!=", token::Type::not_equal },
      { "<", token::Type::less },
      { "<=", token::Type::less_or_equal },
      { ">", token::Type::greater },
      { ">=", token::Type::greater_or_equal },
    };


};

#endif
