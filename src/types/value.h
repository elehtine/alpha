#ifndef VALUE_H
#define VALUE_H

#include <string>


namespace value {
  
  class Value {
    public:
      virtual operator int() = 0;
      virtual operator bool() = 0;
      virtual operator std::string() = 0;
  };

  class Integer: public Value {
    public:
      Integer(int value);
      operator int() override;
      operator bool() override;
      operator std::string() override;

    private:
      int value;
  };

  class Boolean: public Value {
    public:
      Boolean(bool value);
      operator int() override;
      operator bool() override;
      operator std::string() override;

    private:
      bool value;
  };

} /* value */ 

#endif
