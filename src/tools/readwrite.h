#ifndef READWRITE_H
#define READWRITE_H

#include <string>
#include <vector>
#include <memory>
#include <filesystem>

#include "../types/token.h"
#include "../types/ast.h"
#include "../types/ir.h"
#include "../types/interpretation.h"

#include "exceptions.h"


class Source;

std::vector<std::string> test_files();

bool user_approval(std::string prompt);

bool is_file(const std::string& file);

std::string read(const std::string& file);

void write(const std::string& file, const std::string& content);

class Printer {
  public:
    virtual void print_lines(Source* source) = 0;
    virtual void print_tokens(std::vector<Token*> tokens) = 0;
    virtual void print_tree(Expression* root) = 0;
    virtual void print_ir(std::vector<Instruction*> ir) = 0;
    virtual void print_asm(std::vector<std::string> lines) = 0;

    virtual void print_interpretation(
        std::vector<Interpretation*> interpretations) = 0;
    virtual void print_check(bool check) = 0;

    virtual void print_exception(const CompileException& exception) = 0;

    virtual void print(std::string message) = 0;
};

class UserPrinter: public Printer  {
  public:
    void print_lines(Source* source) override;
    void print_tokens(std::vector<Token*> tokens) override;
    void print_tree(Expression* root) override;
    void print_ir(std::vector<Instruction*> ir) override;
    void print_asm(std::vector<std::string> lines) override;

    virtual void print_interpretation(
        std::vector<Interpretation*> interpretations) override;
    void print_check(bool check) override;

    void print_exception(const CompileException& exception) override;

    void print(std::string message) override;
};

enum class FileType {
  source,
  tokens,
  tree,
  internal,
  assembly,
  interpret,
  check,
  exception,
};

class FilePrinter: public Printer  {
  public:
    FilePrinter(std::string name);

    void print_lines(Source* source) override;
    void print_tokens(std::vector<Token*> tokens) override;
    void print_tree(Expression* root) override;
    void print_ir(std::vector<Instruction*> ir) override;
    void print_asm(std::vector<std::string> lines) override;

    virtual void print_interpretation(
        std::vector<Interpretation*> interpretations) override;
    void print_check(bool check) override;

    void print_exception(const CompileException& exception) override;

    void print(std::string message) override;

  private:
    std::string filename(const FileType type);
    void print(const std::string& result, const std::string& file);
    bool accept(const std::string& result, const std::string& before);

    const std::string name;

    std::string input;
};

#endif
