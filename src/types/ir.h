#ifndef IR_H
#define IR_H

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "location.h"

class IrVar {
  public:
    IrVar();
    IrVar(Location location, std::string name);
    operator std::string() const;
    friend bool operator<(const IrVar& left, const IrVar& right);

  private:
    Location location;
    std::string name;
};


class Locals;
class AssemblyGenerator;

class Instruction {
  public:
    virtual ~Instruction() = default;
    virtual operator std::string() const = 0;

    virtual void add_variables(Locals* locals) const = 0;
    virtual void to_asm(AssemblyGenerator* asm_generator) const = 0;

  protected:
    std::string format(std::vector<std::string> values) const;
    std::string format(std::vector<IrVar> values) const;
    std::string move(std::string from, std::string to) const;
    const std::string stack_pointer = "%rsp";
    const std::string register_a = "%rax";
    const std::string register_source = "%rsi";
    const std::string register_destination = "%rdi";
};

class LoadIntConst : public Instruction {
  public:
    LoadIntConst(int value, const IrVar& destination);
    operator std::string() const override;

    void add_variables(Locals* locals) const override;
    void to_asm(AssemblyGenerator* asm_generator) const override;

  private:
    int value;
    IrVar destination;
};

class Copy : public Instruction {
  public:
    Copy(IrVar source, IrVar destination);
    operator std::string() const override;

    void add_variables(Locals* locals) const override;
    void to_asm(AssemblyGenerator* asm_generator) const override;

  private:
    IrVar source;
    IrVar destination;
};

class Call : public Instruction {
  public:
    Call(IrVar function, std::vector<IrVar> arguments, IrVar destination);
    operator std::string() const override;

    void add_variables(Locals* locals) const override;
    void to_asm(AssemblyGenerator* asm_generator) const override;

  private:
    IrVar function;
    std::vector<IrVar> arguments;
    IrVar destination;
};

class Label : public Instruction {
  public:
    Label(int value);
    operator std::string() const override;

    void add_variables(Locals* locals) const override;
    void to_asm(AssemblyGenerator* asm_generator) const override;

  private:
    int value;
};

class CondJump : public Instruction {
  public:
    CondJump(IrVar condition, Label* then_label,
        Label* else_label);
    operator std::string() const override;

    void add_variables(Locals* locals) const override;
    void to_asm(AssemblyGenerator* asm_generator) const override;

  private:
    IrVar condition;
    Label* then_label;
    Label* else_label;
};

class Jump : public Instruction {
  public:
    Jump(Label* label);
    operator std::string() const override;

    void add_variables(Locals* locals) const override;
    void to_asm(AssemblyGenerator* asm_generator) const override;

  private:
    Label* label;
};

#endif
