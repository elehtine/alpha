#ifndef TOKENISER_H
#define TOKENISER_H

#include <string>
#include <vector>

namespace tokeniser {

  enum class Type {
    literal,
    identifier,
    end,
  };

  class Token {
    public:
      Token(Type type, std::string content);

    private:
      Type type;
      std::string content;
  };


  std::vector<Token> tokenise(const std::string& filename);

} /* tokeniser */

#endif
