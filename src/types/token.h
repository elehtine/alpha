#ifndef TOKEN_H
#define TOKEN_H

#include <regex>
#include <vector>
#include <map>
#include <memory>

#include "../tools/exceptions.h"

class Printer;

class Location {
  public:
    Location(int row, int column, std::string line);
    operator std::string() const;
    TokeniseException error();
    std::string error_mark();

  private:
    const int row;
    const int column;
    const std::string line;
};

namespace token {

  enum class Type {
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

} /* token */

const int expression = 0;
const int assignment = 1;
const int logical_or = 2;
const int logical_and = 3;
const int equality = 4;
const int comparison = 5;
const int term = 6;
const int factor = 7;
const int primary = 8;
const int unknown = 9;

std::string to_string(const token::Type& type);

class Token {
  public:
    Token(token::Type type, std::string content, Location location);
    operator std::string() const;
    bool match(token::Type match_type);
    virtual int level() const;
    ParseException error(std::vector<token::Type> types);

    std::string get_content() const;
    std::string parse_str();

  private:
    token::Type type;
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
    bool match(token::Type match_type);
    bool match(std::vector<token::Type> types);

    ParseException error(std::vector<token::Type> types);
    ParseException error(token::Type type);

  private:
    Printer* printer;

    std::vector<std::unique_ptr<Token>> tokens;
    size_t position = 0;
};

#endif
