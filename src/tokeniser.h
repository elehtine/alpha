#ifndef TOKENISER_H
#define TOKENISER_H

#include <string>
#include <vector>
#include <regex>

#include "tools/exceptions.h"

namespace tokeniser {

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

  class Tokeniser {
    public:
      Tokeniser(const std::string& content);
      std::vector<Token> get_tokens() const;
      std::string prefix() const;
      operator std::string();

    private:
      void tokenise();
      bool check(const std::regex& expression, const Type& type);

      std::vector<Token> tokens;
      const std::string content;

      std::size_t position = 0;
      std::string error = "";

      std::vector<std::pair<std::regex, Type>> types = {
        { std::regex("^\\s+"), Type::whitespace },
        { std::regex("^(\\(|\\))"), Type::punctuation },
        { std::regex("^(\\+|-|\\*|/)"), Type::oper },
        { std::regex("^[a-zA-Z]\\w*"), Type::identifier },
        { std::regex("^\\d+"), Type::literal },
      };
  };

} /* tokeniser */

#endif
