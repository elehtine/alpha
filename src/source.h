#ifndef SOURCE_H
#define SOURCE_H

#include <string>

#include "tools/readwrite.h"

class Source {
  public:
    Source(std::string source, Printer* printer);
    operator std::string() const;

    int size() const;
    std::string line(int index) const;
    std::string line(int index, int from, int to) const;

  private:
    std::vector<std::string> split(std::string);

    std::vector<std::string> lines;
};

#endif
