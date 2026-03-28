#include <iostream>
#include <string>
#include <vector>
#include <sstream>

int main() {
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  while (true) {
    std::cout << "$ ";

    std::string input;
    if (!std::getline(std::cin, input)) {
      break; 
    }

    if (input.empty()) continue;

    if (input == "exit 0" || input == "exit") {
      return 0; // gtfo, holly shit
    } else if (input.substr(0, 5) == "echo ") {
      std::cout << input.substr(5) << std::endl;
    } else if (input.substr(0, 5) == "type ") {
      std::string arg = input.substr(5);
      if (arg == "exit" || arg == "echo" || arg == "type") {
        std::cout << arg << " is a shell builtin" << std::endl;
      } else {
        std::cout << arg << ": not found" << std::endl; // wtf is this
      }
    } else {
      std::cout << input << ": command not found" << std::endl;
    }
  }
  
  return 0;
}
