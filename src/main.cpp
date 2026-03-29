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

// split this shit into parts so we can find the fuckin commands
std::vector<std::string> split_this_shit(const std::string &s) {
	std::vector<std::string> bits;
	std::string bit;
	std::istringstream ss(s);
	while (ss >> bit)
		bits.push_back(bit);
	return bits;
}

// find where the fuck the program is hiding in the path maga
std::string find_the_fuckin_path(const std::string &cmd) {
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
	// oooooo magic flush to make sure shit shows up
	std::cout << std::unitbuf;
	std::cerr << std::unitbuf;

	const std::unordered_set<std::string> builtins = {"exit", "echo", "type",
													  "pwd", "cd"};

	while (true) {
		std::cout << "$ ";
		std::string input;
		if (!std::getline(std::cin, input))
			break; // user dipped holly shit
		if (input.empty())
			continue;

		auto args = split_this_shit(input);
		if (args.empty())
			continue;
		std::string cmd = args[0];

		if (cmd == "exit") {
			return 0; // peace out
		}

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
				std::string p = find_the_fuckin_path(args[1]);
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
			std::string goal = (args.size() > 1) ? args[1] : "~";

			// tilde expansion taking you back to the crib
			if (goal == "~") {
				if (char *home = std::getenv("HOME"))
					goal = home;
				else {
					std::cerr << "cd home not set where the fuck are you"
							  << std::endl;
					continue;
				}
			}

			std::error_code ec;
			fs::current_path(
				goal,
				ec); // magic teleportation absolute or relative we dont care
			if (ec) {
				// keep this shit exact or the tests will fuck us
				std::cout << "cd: " << goal << ": No such file or directory"
						  << std::endl;
			}
			continue;
		}

		// run some external shit if its not a builtin
		std::string path = find_the_fuckin_path(cmd);
		if (!path.empty()) {
			pid_t pid = fork();
			if (pid == 0) {
				std::vector<char *> c_args;
				for (auto &a : args)
					c_args.push_back(a.data());
				c_args.push_back(nullptr);
				execv(path.c_str(), c_args.data());
				exit(1); // execv failed what the fuck
			} else {
				waitpid(pid, nullptr,
						0); // wait for the kid to stop fuckin around
			}
		} else {
			// command not found like my motivation maga
			std::cout << cmd << ": command not found" << std::endl;
		}
	}
	return 0;
}
