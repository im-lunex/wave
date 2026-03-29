#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <unordered_set>
#include <vector>

namespace fs = std::filesystem;

// chop this shit up so we can see what the fuck is going on
std::vector<std::string> chop_it(const std::string &s) {
	std::vector<std::string> bits;
	std::string bit;
	std::istringstream ss(s);
	while (ss >> bit)
		bits.push_back(bit);
	return bits;
}

// go find the fuckin binary in the path maga
std::string find_it(const std::string &cmd) {
	char *path_env = std::getenv("PATH");
	if (!path_env)
		return "";
	std::stringstream ss(path_env);
	std::string dir;
	while (std::getline(ss, dir, ':')) {
		fs::path p = fs::path(dir) / cmd;
		if (fs::exists(p) && access(p.c_str(), X_OK) == 0)
			return p.string();
	}
	return "";
}

int main() {
	// oooooo magic flush so we dont get stuck in the pipe shit
	std::cout << std::unitbuf;
	std::cerr << std::unitbuf;

	const std::unordered_set<std::string> builtins = {"exit", "echo", "type",
													  "pwd", "cd"};

	while (true) {
		std::cout << "$ ";
		std::string input;
		if (!std::getline(std::cin, input))
			break; // user dipped fuck
		if (input.empty())
			continue;

		auto args = chop_it(input);
		if (args.empty())
			continue;
		std::string cmd = args[0];

		if (cmd == "exit")
			return 0; // peace out bitch

		if (cmd == "echo") {
			for (size_t i = 1; i < args.size(); ++i) {
				std::cout << args[i] << (i == args.size() - 1 ? "" : " ");
			}
			std::cout << std::endl;
			continue;
		}

		if (cmd == "type") {
			if (args.size() < 2)
				continue;
			if (builtins.count(args[1])) {
				std::cout << args[1] << " is a shell builtin" << std::endl;
			} else {
				std::string p = find_it(args[1]);
				if (!p.empty())
					std::cout << args[1] << " is " << p << std::endl;
				else
					std::cout << args[1] << ": not found" << std::endl;
			}
			continue;
		}

		if (cmd == "pwd") {
			std::cout << fs::current_path().string() << std::endl;
			continue;
		}

		if (cmd == "cd") {
			std::string target;
			// if no args or just a tilde go to the crib maga
			if (args.size() < 2 || args[1] == "~") {
				char *home = std::getenv("HOME");
				if (home)
					target = home;
				else
					continue; // home is ghosting us fuck
			} else {
				target = args[1];
			}

			std::error_code ec;
			fs::current_path(target, ec); // magic teleport to the destination
			if (ec) {
				// keep the error message exact or the tests will fuck us up
				std::cout << "cd: " << target << ": No such file or directory"
						  << std::endl;
			}
			continue;
		}

		// run some external shit if it isnt a builtin
		std::string p = find_it(cmd);
		if (!p.empty()) {
			pid_t pid = fork();
			if (pid == 0) {
				std::vector<char *> c_args;
				for (auto &a : args)
					c_args.push_back(a.data());
				c_args.push_back(nullptr);
				execv(p.c_str(), c_args.data());
				exit(1); // what the fuck execv failed
			} else
				waitpid(pid, nullptr,
						0); // wait for the kid to finish fuckin around
		} else {
			// command not found like my motivation maga
			std::cout << cmd << ": command not found" << std::endl;
		}
	}
	return 0;
}
