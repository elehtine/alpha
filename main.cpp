#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "src/command.h"


int main(int argc, char* argv[]) {
  std::vector<std::unique_ptr<Command>> commands;
  commands.push_back(std::unique_ptr<Command>(new Compile));
  commands.push_back(std::unique_ptr<Command>(new Test));
  commands.push_back(std::unique_ptr<Command>(new Help));

  for (std::unique_ptr<Command>& command: commands) {
    if (command->check(argc, argv)) {
      command->execute();
      break;
    }
  }
}
