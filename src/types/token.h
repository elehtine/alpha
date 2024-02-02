#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <vector>

namespace token {

  enum class Type {
    whitespace,
    punctuation,
    oper,
    identifier,
    literal,
    eof,
  };

  std::string to_string(const Type& type);

  class Token {
    public:
      Token(Type type, std::string content);
      operator std::string() const;
      Type get_type() const;

      int parse_int();
      std::string parse_str();

    private:
      Type type;
      std::string content;

      std::string message(std::vector<Type> need);
  };


} /* token */

#endif
