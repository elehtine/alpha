#ifndef TOKENISER_H
#define TOKENISER_H

#include <string>
#include <vector>

namespace tokeniser {

  enum class Type {
    literal,
    identifier,
    eol,
    end,
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


  void tokenise(const std::string& filename);

} /* tokeniser */

#endif
