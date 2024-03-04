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

#endif
