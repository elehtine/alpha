#ifndef TOKEN_H
#define TOKEN_H

#include <regex>
#include <vector>
#include <map>
#include <memory>


class Parser;

namespace ast {

  class Expression;

} /* ast */

namespace token {

  class Location {
    public:
      Location(int line, int column);
      operator std::string() const;

    private:
      int line;
      int column;
  };

  enum class Type {
    whitespace,

    left_parenthesis, right_parenthesis,
    left_brace, right_brace,
    dot, comma,
    colon, semicolon,

    plus, minus, product, division, modulo,

    bang, equal,
    equal_equal, not_equal,
    less, less_or_equal,
    greater, greater_or_equal,

    keyword, identifier, literal,

    eof,
  };

  const int expression = 0;
  const int term = 1;
  const int factor = 2;
  const int primary = 3;
  const int unknown = 4;

  std::string to_string(const Type& type);

  class Token {
    public:
      Token(Type type, std::string content, Location location);
      operator std::string() const;
      std::string get_content() const;

      std::string parse_str();
      bool match(std::vector<Type> types);

      virtual int level() const;
      std::string message(std::vector<Type> need) const;

    protected:
      Type type;
      std::string content;
      Location location;
  };

} /* token */

#endif
