#ifndef IR_H
#define IR_H

#include <string>
#include <vector>
#include <map>


class IrVar {
  public:
    IrVar(std::string name);
    operator std::string() const;
    friend bool operator<(const IrVar& left, const IrVar& right);

  private:
    std::string name;
};


class Locals;

class Instruction {
  public:
    virtual ~Instruction() = default;
    virtual operator std::string() const = 0;

    virtual void add_variables(Locals* locals) const = 0;

  protected:
    std::string format(std::vector<std::string> values) const;
    std::string format(std::vector<IrVar> values) const;
};

class LoadIntConst : public Instruction {
  public:
    LoadIntConst(int value, const IrVar& destination);
    operator std::string() const override;

    void add_variables(Locals* locals) const override;

  private:
    int value;
    IrVar destination;
};

class Copy : public Instruction {
  public:
    operator std::string() const override;

    void add_variables(Locals* locals) const override;

  private:
    IrVar source;
    IrVar destination;
};

class Call : public Instruction {
  public:
    Call(IrVar function, std::vector<IrVar> arguments, IrVar destination);
    operator std::string() const override;

    void add_variables(Locals* locals) const override;

  private:
    IrVar function;
    std::vector<IrVar> arguments;
    IrVar destination;
};

#endif
