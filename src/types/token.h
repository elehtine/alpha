#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <vector>
#include <memory>
#include <regex>


namespace ast {

  class Expression;

} /* ast  */

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
      std::string get_content() const;

      std::string parse_str();

      std::unique_ptr<ast::Expression> parse() const;


    private:
      Type type;
      std::string content;

      std::string message(std::vector<Type> need) const;
  };

  class Whitespace: public Token {
    public:
      Whitespace(Type type, std::string content);

      static const std::regex expression;
  };

  class Punctuation: public Token {
    public:
      Punctuation(Type type, std::string content);

      static const std::regex expression;
  };

  class Oper: public Token {
    public:
      Oper(Type type, std::string content);

      static const std::regex expression;
  };

  class Identifier: public Token {
    public:
      Identifier(Type type, std::string content);

      static const std::regex expression;
  };

  class Literal: public Token {
    public:
      Literal(Type type, std::string content);

      static const std::regex expression;
  };

  class Eof: public Token {
    public:
      Eof(Type type, std::string content);
  };

} /* token */

#endif
