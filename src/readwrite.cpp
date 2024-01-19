#include <fstream>
#include <string>


std::string read(const std::string& file) {
  std::ifstream in;
  std::string text;
  std::string s;
  in.open(file, std::ios::in);
  while (getline(in, s)) {
    text.append(s);
    text.append("\n");
  }
  return text;
}

void write(const std::string& file, const std::string& content) {
  std::ofstream out(file);
  out << content;
  out.close();
}
