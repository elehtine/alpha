#ifndef TOKENISER_H
#define TOKENISER_H

#include "types/token.h"
#include "types/source.h"


std::unique_ptr<Tokens> tokenise(Source& source, Printer* printer);

class Tokeniser {
  public:
    Tokeniser(Source& source, Printer* printer);
    std::unique_ptr<Tokens> tokenise();

  private:
    std::unique_ptr<Token> scan_token();
    std::unique_ptr<Token> create_token(token::Type type, std::string content);
    bool check(const std::regex& expression);

    Source source;
    Printer* printer;

    int line;
    int column;
    int last_column;

    const std::regex whitespace { "^(\\s)+" };
    const std::regex comment { "^\\s*(//|#)[^\\\\]+" };
    const std::regex punctuation { "^(\\(|\\)|\\{|\\}|,|:|;)" };
    const std::regex oper { "^(\\+|-|\\*|/|%|=|==|!=|<|<=|>|>=)" };
    const std::regex keyword {
      "^(var|Int|Bool|true|false|if|then|else|while|do|read_int|print_int)"
    };
    const std::regex identifier { "^[a-zA-Z_]+" };
    const std::regex literal { "^\\d+" };

    std::map<std::string, token::Type> str_to_type = {
      { "(", token::Type::left_parenthesis },
      { ")", token::Type::right_parenthesis },
      { "{", token::Type::left_brace },
      { "}", token::Type::right_brace },
      { ".", token::Type::dot },
      { ",", token::Type::comma },
      { ":", token::Type::colon },
      { ";", token::Type::semicolon },
      { "+", token::Type::plus },
      { "-", token::Type::minus },
      { "*", token::Type::product },
      { "/", token::Type::division },
      { "%", token::Type::modulo },
      { "!", token::Type::keyword_not },
      { "=", token::Type::equal },
      { "==", token::Type::equal },
      { "!=", token::Type::not_equal },
      { "<", token::Type::less },
      { "<=", token::Type::less_or_equal },
      { ">", token::Type::greater },
      { ">=", token::Type::greater_or_equal },
      { "var", token::Type::var },
      { "Int", token::Type::integer },
      { "Bool", token::Type::boolean },
      { "true", token::Type::keyword_true },
      { "false", token::Type::keyword_false },
      { "if", token::Type::keyword_if },
      { "then", token::Type::keyword_then },
      { "else", token::Type::keyword_else },
      { "while", token::Type::keyword_while },
      { "do", token::Type::keyword_do },
      { "read_int", token::Type::read_int },
      { "print_int", token::Type::print_int },
    };

};

#endif
