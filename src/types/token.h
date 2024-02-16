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

  const int statement = 0;
  const int expression = 1;
  const int term = 2;
  const int factor = 3;
  const int primary = 4;
  const int unknown = 5;

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

  class Whitespace: public Token {
    public:
      Whitespace(Type type, std::string content, Location location);

      static const std::regex expression;
  };

  class Comment: public Token {
    public:
      Comment(Type type, std::string content, Location location);

      static const std::regex expression;
  };

  class Punctuation: public Token {
    public:
      Punctuation(Type type, std::string content, Location location);
      std::unique_ptr<ast::Expression> parse(Parser* parser) const override;

      static const std::regex expression;
  };

  class Oper: public Token {
    public:
      Oper(Type type, std::string content, Location location);
      int level() const override;

      static const std::regex expression;
  };

  class Identifier: public Token {
    public:
      Identifier(Type type, std::string content, Location location);
      std::unique_ptr<ast::Expression> parse(Parser* parser) const override;
      int level() const override;

      static const std::regex expression;
  };

  class Keyword: public Token {
    public:
      Keyword(Type type, std::string content, Location location);
      std::unique_ptr<ast::Expression> parse(Parser* parser) const override;
      int level() const override;

      static const std::regex expression;
  };

  class Literal: public Token {
    public:
      Literal(Type type, std::string content, Location location);
      std::unique_ptr<ast::Expression> parse(Parser* parser) const override;
      int level() const override;

      static const std::regex expression;
  };

  class Eof: public Token {
    public:
      Eof(Type type, std::string content, Location location);
  };

} /* token */

#endif
