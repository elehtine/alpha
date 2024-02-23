#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <vector>
#include <memory>
#include <regex>


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
    comment,
    punctuation,
    oper,
    identifier,
    keyword,
    literal,
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
      Type get_type() const;
      std::string get_content() const;

      std::string parse_str();

      virtual std::unique_ptr<ast::Expression> parse(Parser* parser) const;
      virtual int level() const;
      std::string message(std::vector<Type> need) const;

    protected:
      Type type;
      std::string content;
      Location location;
  };

} /* token */

#endif
