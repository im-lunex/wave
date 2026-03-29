#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

namespace fs = std::filesystem;

// split this shit into parts, and hope there are no quotes (yet)
std::vector<std::string> split(const std::string &s) {
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (tokenStream >> token) {
		tokens.push_back(token);
	}
	return tokens;
}

// find where the fuck the program is hiding in the PATH
std::string find_path(const std::string &command) {
	char *path_env = std::getenv("PATH");
	if (!path_env)
		return "";

	std::stringstream ss(path_env);
	std::string path;
	while (std::getline(ss, path, ':')) {
		fs::path p = fs::path(path) / command;
		if (fs::exists(p) && access(p.c_str(), X_OK) == 0)
			return p.string();
	}
	return "";
}

int main() {
	// flush it like a toilet
	std::cout << std::unitbuf;
	std::cerr << std::unitbuf;

	while (true) {
		std::cout << "$ ";

		std::string input;
		if (!std::getline(std::cin, input))
			break;
		if (input.empty())
			continue;

		std::vector<std::string> args = split(input);
		if (args.empty())
			continue;

		std::string cmd = args[0];

		if (cmd == "exit") {
			return 0; // gtfo
		} else if (cmd == "echo") {
			for (size_t i = 1; i < args.size(); ++i) {
				std::cout << args[i] << (i == args.size() - 1 ? "" : " ");
			}
			std::cout << std::endl;
		} else if (cmd == "type") {
			if (args.size() < 2) {
				std::cout << "type: what do you want me to type?" << std::endl;
				continue;
			}
			std::string arg = args[1];
			if (arg == "exit" || arg == "echo" || arg == "type" ||
				arg == "pwd" || arg == "cd") {
				std::cout << arg << " is a shell builtin" << std::endl;
			} else {
				std::string path = find_path(arg);
				if (!path.empty()) {
					std::cout << arg << " is " << path << std::endl;
				} else {
					std::cout << arg << ": not found" << std::endl;
				}
			}
		} else if (cmd == "pwd") {
			std::cout << fs::current_path().string() << std::endl;
		} else if (cmd == "cd") {
			std::string target = (args.size() < 2) ? "~" : args[1];

			// handle the home sweet home shortcut
			if (target == "~") {
				char *home = std::getenv("HOME");
				if (home) {
					target = home;
				} else {
					// no home? that's rough buddy.
					std::cerr << "cd: HOME not set? Are you homeless?"
							  << std::endl;
					continue;
				}
			}
			try {
				// teleporting... hope we don't end up in a wall
				fs::current_path(target);
			} catch (...) {
				// if we are here, it means we failed to teleport
				std::cout << "cd: " << target << ": No such file or directory"
						  << std::endl;
			}
		} else {
			std::string path = find_path(cmd);
			if (!path.empty()) {
				// holly shit, run it
				pid_t pid = fork();
				if (pid == 0) {
					std::vector<char *> c_args;
					for (auto &a : args)
						c_args.push_back(&a[0]);
					c_args.push_back(nullptr);
					execv(path.c_str(), c_args.data());
					// if we are here, something went horribly wrong
					std::cerr << "execv failed: " << path << " is a brick?"
							  << std::endl;
					exit(1);
				} else {
					waitpid(pid, nullptr, 0);
				}
			} else {
				// command is nowhere to be found, like my motivation
				std::cout << cmd << ": command not found" << std::endl;
			}
		}
	}

	return 0;
}
