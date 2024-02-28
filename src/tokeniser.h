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
    std::unique_ptr<Token> create_token(TokenType type, std::string content);
    bool check(const std::regex& expression);

    Source source;
    Printer* printer;

    int line;
    int column;
    int last_column;

    const std::regex whitespace { "^\\s+" };
    const std::regex comment { "^(//|#)[^(\\n)]+" };
    const std::regex punctuation { "^(\\(|\\)|\\{|\\}|,|:|;)" };
    const std::regex oper { "^(\\+|-|\\*|/|%|==|!=|<=|>=|=|<|>|and|or)" };
    const std::regex keyword {
      "^(var|true|false|if|then|else|while|do)"
    };
    const std::regex identifier { "^[a-zA-Z_][a-zA-Z_0-9]*" };
    const std::regex literal { "^(\\d)+" };

    std::map<std::string, TokenType> str_to_type = {
      { "(", TokenType::left_parenthesis },
      { ")", TokenType::right_parenthesis },
      { "{", TokenType::left_brace },
      { "}", TokenType::right_brace },
      { ".", TokenType::dot },
      { ",", TokenType::comma },
      { ":", TokenType::colon },
      { ";", TokenType::semicolon },
      { "+", TokenType::plus },
      { "-", TokenType::minus },
      { "*", TokenType::product },
      { "/", TokenType::division },
      { "%", TokenType::modulo },
      { "!", TokenType::keyword_not },
      { "=", TokenType::equal },
      { "==", TokenType::equal_equal },
      { "!=", TokenType::not_equal },
      { "<", TokenType::less },
      { "<=", TokenType::less_or_equal },
      { ">", TokenType::greater },
      { ">=", TokenType::greater_or_equal },
      { "and", TokenType::logical_and },
      { "or", TokenType::logical_or },
      { "var", TokenType::var },
      { "Int", TokenType::integer },
      { "Bool", TokenType::boolean },
      { "true", TokenType::keyword_true },
      { "false", TokenType::keyword_false },
      { "if", TokenType::keyword_if },
      { "then", TokenType::keyword_then },
      { "else", TokenType::keyword_else },
      { "while", TokenType::keyword_while },
      { "do", TokenType::keyword_do },
    };

};

#endif
