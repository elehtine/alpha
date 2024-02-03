#ifndef IR_H
#define IR_H

#include <string>
#include <vector>


class IrVar {
  public:
    IrVar(std::string name);
    operator std::string() const;

  private:
    std::string name;
};


class Instruction {
  public:
    virtual operator std::string() const = 0;
  protected:
    std::string format(std::vector<std::string> values) const;
    std::string format(std::vector<IrVar> values) const;
};

class LoadIntConst : public Instruction {
  public:
    operator std::string() const override;
  private:
    int value;
    IrVar destination;
};

class Copy : public Instruction {
  public:
    operator std::string() const override;
  private:
    IrVar source;
    IrVar destination;
};

class Call : public Instruction {
  public:
    operator std::string() const override;
  private:
    IrVar function;
    std::vector<IrVar> arguments;
    IrVar destination;
};


class SymbolTable {
};

#endif
