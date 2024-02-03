#ifndef READWRITE_H
#define READWRITE_H

#include <string>
#include <vector>
#include <memory>
#include <filesystem>

#include "../types/token.h"
#include "../types/ast.h"
#include "../types/interpretation.h"


std::vector<std::string> test_files();

bool user_approval(std::string prompt);

bool is_file(const std::string& file);

std::string read(const std::string& file);

void write(const std::string& file, const std::string& content);

class Printer {
  public:
    virtual void print_source(std::string) = 0;
    virtual void print_tokens(std::vector<token::Token> tokens) = 0;
    virtual void print_tree(ast::Expression* root) = 0;
    virtual void print_value(interpretation::Interpretation* interpretation) = 0;
};

class UserPrinter: public Printer  {
  public:
    void print_source(std::string source) override;
    void print_tokens(std::vector<token::Token> tokens) override;
    void print_tree(ast::Expression* root) override;
    void print_value(interpretation::Interpretation* interpretation) override;
};

enum class FileType {
  source,
  tokens,
  tree,
  interpret,
};

class FilePrinter: public Printer  {
  public:
    FilePrinter(std::string name);

    void print_source(std::string source) override;
    void print_tokens(std::vector<token::Token> tokens) override;
    void print_tree(ast::Expression* root) override;
    void print_value(interpretation::Interpretation* interpretation) override;

  private:
    std::string filename(const FileType type);
    void print(const std::string& result, const std::string& file);
    bool accept(const std::string& result, const std::string& before);

    const std::string name;

    std::string input;
};

#endif
