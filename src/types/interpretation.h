#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <memory>
#include <map>


class Interpretation {
  public:
    virtual operator int() const = 0;
    virtual operator bool() const = 0;
    virtual operator std::string() const = 0;
    virtual std::unique_ptr<Interpretation> clone() const = 0;
};

class Integer: public Interpretation {
  public:
    Integer(int value);
    operator int() const override;
    operator bool() const override;
    operator std::string() const override;
    std::unique_ptr<Interpretation> clone() const override;

  private:
    int value;
};

class Boolean: public Interpretation {
  public:
    Boolean(bool value);
    operator int() const override;
    operator bool() const override;
    operator std::string() const override;
    std::unique_ptr<Interpretation> clone() const override;

  private:
    bool value;
};

class SymTab {
  public:
    SymTab(std::unique_ptr<SymTab> parent);

    void assign_variable(std::string identifier, std::unique_ptr<Interpretation> value);
    std::unique_ptr<Interpretation> get_variable(std::string identifier);

    std::unique_ptr<SymTab> get_parent();

  private:
    std::unique_ptr<SymTab> parent;
    std::map<std::string, std::unique_ptr<Interpretation>> symbols;
};

#endif
