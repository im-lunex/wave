#include <iostream>
#include <string>

int main() {
  // Flush after every std::cout / std:cerr, ooowww maaa ga
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  while (true) {
    std::cout << "$ "; // show the prompt, wtf

    std::string input;
    if (!std::getline(std::cin, input)) {
      break; // holly shit, input ended
    }

    if (input == "exit 0") {
      return 0; // gtfo, we are done here
    }

    if (!input.empty()) {
      std::cout << input << ": command not found" << std::endl; // shit, command is missing
    }
  }
  
  return 0;
}
