#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
#include <memory>
#include <vector>

class Expression;
class Interpretation;
class Printer;

class Interpreter {
  public:
    Interpreter(Expression* tree, Printer* printer);
    void add_interpretation(std::unique_ptr<Interpretation> value);

  private:
    std::vector<Interpretation*> get_interpretations();

    Expression* tree;
    Printer* printer;

    std::vector<std::unique_ptr<Interpretation>> interpretations;
};

#endif
