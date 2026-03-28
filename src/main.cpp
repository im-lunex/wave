#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <unistd.h>

namespace fs = std::filesystem;

// find where the fuck the program is
std::string find_path(const std::string& command) {
    char* path_env = std::getenv("PATH");
    if (!path_env) return "";

    std::stringstream ss(path_env);
    std::string path;
    while (std::getline(ss, path, ':')) {
        fs::path p = fs::path(path) / command;
        if (fs::exists(p) && access(p.c_str(), X_OK) == 0) return p.string();
    }
    return "";
}

int main() {
	std::cout << std::unitbuf;
	std::cerr << std::unitbuf;

	while (true) {
		std::cout << "$ ";

		std::string input;
		if (!std::getline(std::cin, input))
			break;
		if (input.empty())
			continue;

		if (input == "exit 0" || input == "exit") {
			return 0; // gtfo
		} else if (input.substr(0, 5) == "echo ") {
			std::cout << input.substr(5) << std::endl;
		} else if (input.substr(0, 5) == "type ") {
			std::string arg = input.substr(5);
			if (arg == "exit" || arg == "echo" || arg == "type") {
				std::cout << arg << " is a shell builtin" << std::endl;
			} else {
				std::string path = find_path(arg);
				if (!path.empty()) {
					std::cout << arg << " is " << path << std::endl;
				} else {
					std::cout << arg << ": not found" << std::endl;
				}
			}
		} else {
			std::cout << input << ": command not found" << std::endl;
		}
	}

	return 0;
}
