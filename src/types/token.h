#ifndef TOKEN_H
#define TOKEN_H

#include <regex>
#include <vector>
#include <map>
#include <memory>

#include "source.h"

#include "../tools/exceptions.h"

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

  std::string to_string(const Type& type);

  const int expression = 0;
  const int term = 1;
  const int factor = 2;
  const int primary = 3;
  const int unknown = 4;

  class Token {
    public:
      Token(Type type, std::string content, Location location);
      operator std::string() const;
      bool match(std::vector<Type> types);
      virtual int level() const;

      std::string get_content() const;
      std::string parse_str();

      std::string message(std::vector<Type> need) const;

    private:
      Type type;
      std::string content;
      Location location;
  };

  class Tokens {
    public:
      Tokens(const Source& source, Printer* printer);
      std::vector<Token*> get_tokens();

      Token* peek() const;
      Token* consume();
      bool match(std::vector<Type> types);

    private:
      void tokenise();
      std::unique_ptr<Token> create_token();
      bool check(const std::regex& expression);

      const Source& source;
      Printer* printer;

      int line;
      int column;

      std::vector<std::unique_ptr<Token>> tokens;
      size_t position = 0;

      const std::regex whitespace { "^(\\s)+" };
      const std::regex comment { "^\\s*(//|#)[^\\\\]+" };
      const std::regex punctuation { "^(\\(|\\)|\\{|\\}|,|:|;)" };
      const std::regex oper { "^(\\+|-|\\*|/|%|=|==|!=|<|<=|>|>=)" };
      const std::regex keyword { "(var|if|then|else|while|do)" };
      const std::regex identifier { "^[a-zA-Z_]+" };
      const std::regex literal { "^\\d+" };

      std::map<std::string, Type> str_to_type {
        { "(", Type::left_parenthesis },
          { ")", Type::right_parenthesis },
          { "{", Type::left_brace },
          { "}", Type::right_brace },
          { ".", Type::dot },
          { ",", Type::comma },
          { ":", Type::colon },
          { ";", Type::semicolon },
          { "+", Type::plus },
          { "-", Type::minus },
          { "*", Type::product },
          { "/", Type::division },
          { "%", Type::modulo },
          { "!", Type::bang },
          { "=", Type::equal },
          { "==", Type::equal },
          { "!=", Type::not_equal },
          { "<", Type::less },
          { "<=", Type::less_or_equal },
          { ">", Type::greater },
          { ">=", Type::greater_or_equal },
      };

  };

} /* token */

#endif
