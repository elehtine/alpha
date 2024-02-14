#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <vector>
#include <memory>
#include <regex>


class Parser;

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

  const int expression = 0;
  const int term = 1;
  const int factor = 2;
  const int primary = 3;
  const int unknown = 4;

  std::string to_string(const Type& type);

  class Token {
    public:
      Token(Type type, std::string content);
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

  };

  class Whitespace: public Token {
    public:
      Whitespace(Type type, std::string content);

      static const std::regex expression;
  };

  class Punctuation: public Token {
    public:
      Punctuation(Type type, std::string content);
      std::unique_ptr<ast::Expression> parse(Parser* parser) const override;

      static const std::regex expression;
  };

  class Oper: public Token {
    public:
      Oper(Type type, std::string content);
      int level() const override;

      static const std::regex expression;
  };

  class Identifier: public Token {
    public:
      Identifier(Type type, std::string content);
      std::unique_ptr<ast::Expression> parse(Parser* parser) const override;
      int level() const override;

      static const std::regex expression;
  };

  class Literal: public Token {
    public:
      Literal(Type type, std::string content);
      std::unique_ptr<ast::Expression> parse(Parser* parser) const override;
      int level() const override;

      static const std::regex expression;
  };

  class Eof: public Token {
    public:
      Eof(Type type, std::string content);
  };

} /* token */

#endif
