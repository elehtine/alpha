#include <iostream>

#include "printer.h"

#include "../types/source.h"
#include "../types/token.h"
#include "../types/ast.h"
#include "../types/value.h"
#include "../types/ir.h"

#include "exceptions.h"
#include "files.h"


template<class T>
std::string to_string(std::vector<T> vec) {
  std::string result = "";
  for (const T& element: vec) {
    result += std::string(element) + "\n";
  }
  return result;
}

template<class T>
std::string to_string(const std::vector<T*> vec) {
  std::string result = "";
  for (const T* element: vec) {
    result += std::string(*element) + "\n";
  }
  return result;
}

std::string bool_to_string(bool value) {
  return value ? "TRUE\n" : "FALSE\n";
}


void UserPrinter::print_lines(Source* source) {
  std::cout << "Lines:" << std::endl;
  std::cout << std::string(*source) << std::endl;
}

void UserPrinter::print_tokens(std::vector<Token*> tokens) {
  std::cout << "Tokens:" << std::endl;
  std::cout << to_string(tokens) << std::endl;
}

void UserPrinter::print_tree(Expression* root) {
  std::cout << "AST:" << std::endl;
  std::cout << root->print(0) << std::endl;
}

void UserPrinter::print_check(Expression* root) {
  std::cout << "AST (checked):" << std::endl;
  std::cout << root->print(0) << std::endl;
}

void UserPrinter::print_ir(std::vector<Instruction*> ir) {
  std::cout << "Internal representation:" << std::endl;
  std::cout << to_string(ir) << std::endl;
}

void UserPrinter::print_asm(std::vector<std::string> lines) {
  std::cout << "Assembly:" << std::endl;
  std::cout << to_string(lines) << std::endl;
}

void UserPrinter::print_interpretation(std::vector<Value> interpretations) {
  std::cout << "Value:" << std::endl;
  std::cout << to_string(interpretations) << std::endl;
}

void UserPrinter::print_exception(const CompileException& exception) {
  std::cout << "Exception:" << std::endl;
  std::cout << exception.what() << std::endl;
}

void UserPrinter::print(std::string message) {
  std::cout << message << std::endl;
}

FilePrinter::FilePrinter(std::string name): name(name) {}

void FilePrinter::print_lines(Source* source) {
  input = *source;
}

void FilePrinter::print_tokens(std::vector<Token*> tokens) {
  print(to_string(tokens), filename(FileType::tokens));
}

void FilePrinter::print_tree(Expression* root) {
  print(root->print(0), filename(FileType::tree));
}

void FilePrinter::print_check(Expression* root) {
  print(root->print(0), filename(FileType::check));
}

void FilePrinter::print_ir(std::vector<Instruction*> ir) {
  print(to_string(ir), filename(FileType::internal));
}

void FilePrinter::print_asm(std::vector<std::string> lines) {
  print(to_string(lines), filename(FileType::assembly));
}

void FilePrinter::print_interpretation(std::vector<Value> interpretations) {
  print(to_string(interpretations), filename(FileType::interpret));
}

void FilePrinter::print_exception(const CompileException& exception) {
  print(exception.what(), filename(FileType::exception));
}

void FilePrinter::print(std::string message) {
  std::cout << message << std::endl;
}

std::string FilePrinter::filename(const FileType type) {
  std::string suffix = ".unknown";
  if (type == FileType::source) suffix = ".alpha";
  if (type == FileType::tokens) suffix = ".tokens";
  if (type == FileType::tree) suffix = ".tree";
  if (type == FileType::internal) suffix = ".internal";
  if (type == FileType::assembly) suffix = ".s";
  if (type == FileType::interpret) suffix = ".interpret";
  if (type == FileType::check) suffix = ".check";
  if (type == FileType::exception) suffix = ".exception";
  return name + suffix;
}

void FilePrinter::print(const std::string& result, const std::string& file) {
  std::string before = read(file);
  if (accept(result, before)) {
    std::cout << "File passed: " << file << std::endl;
    write(file, result);
  } else {
    std::cout << "File failed: " << file << std::endl;
  }
}

bool FilePrinter::accept(const std::string& result, const std::string& before) {
  if (result == before) {
    return true;
  }

  std::cout << "File failed! " << std::endl;
  std::cout << "Input:" << std::endl;
  std::cout << input << std::endl;
  std::cout << "Result:" << std::endl;
  std::cout << result << std::endl;
  std::cout << "Before:" << std::endl;
  std::cout << before << std::endl;
  return user_approval("Do you want to override [Y/n]? ");
}
