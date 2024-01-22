#ifndef TOKENISER_H
#define TOKENISER_H

#include <string>
#include <vector>

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

    private:
      Type type;
      std::string content;
  };


  std::vector<Token> tokenise(const std::string& filename);

} /* tokeniser */

#endif
