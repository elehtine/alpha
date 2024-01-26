#ifndef READWRITE_H
#define READWRITE_H

#include <string>


bool is_file(const std::string& file);

std::string read(const std::string& file);

void write(const std::string& file, const std::string& content);

#endif
