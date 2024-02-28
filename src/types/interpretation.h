#ifndef VALUE_H
#define VALUE_H

#include <string>


class Interpretation {
  public:
    virtual operator int() const = 0;
    virtual operator bool() const = 0;
    virtual operator std::string() const = 0;
};

class Integer: public Interpretation {
  public:
    Integer(int value);
    operator int() const override;
    operator bool() const override;
    operator std::string() const override;

  private:
    int value;
};

class Boolean: public Interpretation {
  public:
    Boolean(bool value);
    operator int() const override;
    operator bool() const override;
    operator std::string() const override;

  private:
    bool value;
};

#endif
