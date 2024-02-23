#ifndef VALUE_H
#define VALUE_H

#include <string>


namespace interpretation {

  class Interpretation {
    public:
      virtual operator int() = 0;
      virtual operator bool() = 0;
      virtual operator std::string() = 0;
  };

  class Integer: public Interpretation {
    public:
      Integer(int value);
      operator int() override;
      operator bool() override;
      operator std::string() override;

    private:
      int value;
  };

  class Boolean: public Interpretation {
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
