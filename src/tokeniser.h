#ifndef TOKENISER_H
#define TOKENISER_H

#include <string>
#include <vector>

#include "tools/exceptions.h"

namespace tokeniser {

  enum class Type {
    eof,
    eol,
    oper,
    identifier,
    literal,
  };

  std::string to_string(Type type);

  class Token {
    public:
      Token(Type type, std::string content);
      operator std::string() const;

      int parse_int();
      std::string parse_str();

    private:
      Type type;
      std::string content;

      std::string message();
  };


  std::vector<Token> tokenise(const std::string& filename);

} /* tokeniser */

#endif
