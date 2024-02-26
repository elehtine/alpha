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
    whitespace,

    left_parenthesis, right_parenthesis,
    left_brace, right_brace,
    dot, comma,
    colon, semicolon,

    plus, minus, product, division, modulo,

    keyword_not, equal,
    equal_equal, not_equal,
    less, less_or_equal,
    greater, greater_or_equal,

    var, integer, boolean, keyword_true, keyword_false,
    keyword_if, keyword_then, keyword_else,
    keyword_while, keyword_do,
    read_int, print_int,

    identifier, literal,

    eof,
  };

} /* token */

const int expression = 0;
const int equality = 1;
const int comparison = 2;
const int term = 3;
const int factor = 4;
const int primary = 5;
const int unknown = 6;

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
