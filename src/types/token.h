#ifndef TOKEN_H
#define TOKEN_H

#include <regex>
#include <vector>
#include <map>
#include <memory>

#include "location.h"


class Printer;
class TokeniseException;
class ParseException;
class FunType;

enum class TokenType {
  left_parenthesis, right_parenthesis,
  left_brace, right_brace,
  dot, comma,
  colon, semicolon,

  plus, minus, product, division, modulo,

  keyword_not, equal,
  equal_equal, not_equal,
  less, less_or_equal,
  greater, greater_or_equal,
  logical_and, logical_or,

  var, integer, boolean, keyword_true, keyword_false,
  keyword_if, keyword_then, keyword_else,
  keyword_while, keyword_do,

  identifier, literal,

  eof,
};

const int expression = 0;
const int assignment = 1;
const int logical_or = 2;
const int logical_and = 3;
const int equality = 4;
const int comparison = 5;
const int term = 6;
const int factor = 7;
const int unary = 8;
const int unknown = 9;

std::string to_string(const TokenType& type);

class Token {
  public:
    Token(TokenType type, std::string content, Location location);
    operator std::string() const;
    bool match(TokenType match_type);
    virtual int level() const;
    ParseException error(std::vector<TokenType> types);
    FunType get_funtype();

    std::string get_content() const;
    std::string parse_str();
    Location copy_location() const;

  private:
    TokenType type;
    std::string content;
    Location location;
};

class Tokens {
  public:
    Tokens(Printer* printer, std::vector<std::unique_ptr<Token>>& tokens);
    std::vector<Token*> get_tokens();

    Token* previous() const;
    Token* peek() const;
    Token* peek_second() const;
    Token* consume();
    bool match(TokenType match_type);
    bool match(std::vector<TokenType> types);

    ParseException error(std::vector<TokenType> types);
    ParseException error(TokenType type);

  private:
    Printer* printer;

    std::vector<std::unique_ptr<Token>> tokens;
    size_t position = 0;
};

#endif
