#include <iostream>
#include <string>

int main() {
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  while (true) {
    std::cout << "$ ";

    std::string input;
    if (!std::getline(std::cin, input)) {
      break; 
    }

    if (input == "exit 0") {
      return 0; // gtfo
    } else if (input.substr(0, 5) == "echo ") {
      std::cout << input.substr(5) << std::endl; // shit, just print it
    } else {
      std::cout << input << ": command not found" << std::endl; // wtf is this command
    }
  }
  
  return 0;
}
