#include <iostream>
#include <string>

int main() {
  // Flush after every std::cout / std:cerr, ooowww maaa ga
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // wtf, REPL starts here
  while (true) {
    std::cout << "$ ";

    std::string input;
    if (!std::getline(std::cin, input)) {
      break; // holly shit, EOF or something
    }

    if (input.empty()) {
      continue; // nothing here, move on shit
    }

    // shit, we don't know any commands yet
    std::cout << input << ": command not found" << std::endl;
  }
  
  return 0;
}
