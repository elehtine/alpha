#ifndef LOCATION_H
#define LOCATION_H

#include <string>

class TokeniseException;


class Location {
  public:
    Location(int row, int column, std::string line);
    operator std::string() const;
    TokeniseException error() const;
    std::string error_mark() const;

  private:
    const int row;
    const int column;
    const std::string line;
};

#endif
