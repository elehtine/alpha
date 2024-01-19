#ifndef COMMAND_H
#define COMMAND_H

#include <string>

class Command {
  public:
    virtual ~Command();

    virtual void execute() = 0;
    virtual bool check(int argc, char* argv[]) = 0;

  protected:
    Command();
};

class Test: public Command {
  public:
    virtual void execute() override;
    virtual bool check(int argc, char* argv[]) override;
};

class Compile: public Command {
  public:
    virtual void execute() override;
    virtual bool check(int argc, char* argv[]) override;

  private:
    std::string filename;
};


class Help: public Command {
  public:
    virtual void execute() override;
    virtual bool check(int argc, char* argv[]) override;
};

#endif
