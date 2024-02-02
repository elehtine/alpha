#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>


namespace interpreter {

  class Value {
    public:
      virtual operator int() = 0;
      virtual operator bool() = 0;
  };

  class Integer: public Value {
    public:
      Integer(int value);
      operator int() override;
      operator bool() override;

    private:
      int value;
  };

  class Boolean: public Value {
    public:
      Boolean(bool value);
      operator int() override;
      operator bool() override;

    private:
      bool value;
  };

  class Interpreter {
    public:
      Interpreter();
      std::string prefix() const;
      operator std::string() const;
  };

}; /* interpreter */

#endif
