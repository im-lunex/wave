#include <iostream>
#include <string>

int main() {
  // Flush after every std::cout / std:cerr, ooowww maaa ga
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  while (true) {
    std::cout << "$ "; // print the fucking prompt

    std::string input;
    if (!std::getline(std::cin, input)) {
      break; // holly shit, input ended
    }
  }
  
  return 0;
}
