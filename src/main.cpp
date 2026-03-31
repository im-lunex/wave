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

std::vector<std::string> chop_it(const std::string &s) {
	std::vector<std::string> bits;
	std::string bit;
	std::istringstream ss(s);
	while (ss >> bit)
		bits.push_back(bit);
	return bits;
}

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
	std::cout << std::unitbuf;
	std::cerr << std::unitbuf;

	const std::unordered_set<std::string> builtins = {
		"exit", "echo", "type", "jobs", "pwd", "cd",
	};

	while (true) {
		std::cout << "$ ";
		std::string input;
		if (!std::getline(std::cin, input))
			break; // user dipped wow magic
		if (input.empty())
			continue;

		auto args = chop_it(input);
		if (args.empty())
			continue;
		std::string cmd = args[0];

		if (cmd == "exit")
			return 0; // but why??

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
			if (args.size() < 2 || args[1] == "~") {
				char *home = std::getenv("HOME");
				if (home)
					target = home;
				else
					continue;
			} else {
				target = args[1];
			}

			std::error_code ec;
			fs::current_path(target, ec);
			if (ec) {
				std::cout << "cd: " << target << ": No such file or directory"
						  << std::endl;
			}
			continue;
		}

		if (cmd == "jobs") {
			continue;
		}

		std::string p = find_it(cmd);
		if (!p.empty()) {
			pid_t pid = fork();
			if (pid == 0) {
				std::vector<char *> c_args;
				for (auto &a : args)
					c_args.push_back(a.data());
				c_args.push_back(nullptr);
				execv(p.c_str(), c_args.data());
				exit(1);
			} else
				waitpid(pid, nullptr, 0);
		} else {
			std::cout << cmd << ": command not found" << std::endl;
		}
	}
	return 0;
}
