#ifndef VALUE_H
#define VALUE_H

namespace value {
  
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

} /* value */ 

#endif
