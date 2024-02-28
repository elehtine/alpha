#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
#include <memory>

class Expression;
namespace interpretation {
  class Interpretation;
}
class Printer;

class Interpreter {
  public:
    Interpreter(Expression* tree, Printer* printer);
    std::unique_ptr<interpretation::Interpretation> interpret();

  private:
    Expression* tree;
    Printer* printer;
};

#endif
