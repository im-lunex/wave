#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <unordered_set>
#include <vector>

namespace fs = std::filesystem;

// keep track of background tasks
struct Job {
	int id;
	pid_t pid;
	std::string command;
};

// global list for our active background jobs
std::vector<Job> active_jobs;
int next_job_id = 1;

// better helper to split string into tokens, handling single and double quotes
std::vector<std::string> chop_it(const std::string &s) {
	std::vector<std::string> bits;
	std::string bit;
	bool in_single_quote = false;
	bool in_double_quote = false;
	bool started = false;

	for (size_t i = 0; i < s.length(); ++i) {
		char c = s[i];

		if (c == '\'' && !in_double_quote) {
			in_single_quote = !in_single_quote;
			started = true;
		} else if (c == '"' && !in_single_quote) {
			in_double_quote = !in_double_quote;
			started = true;
		} else if (std::isspace(c) && !in_single_quote && !in_double_quote) {
			if (started) {
				bits.push_back(bit);
				bit.clear();
				started = false;
			}
		} else {
			bit += c;
			started = true;
		}
	}

	if (started) {
		bits.push_back(bit);
	}

	return bits;
}

// look through system PATH to find where an executable lives
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
	// make sure output shows up immediately
	std::cout << std::unitbuf;
	std::cerr << std::unitbuf;

	const std::unordered_set<std::string> builtins = {
		"exit", "echo", "type", "jobs", "pwd", "cd",
	};

	while (true) {
		int status;
		pid_t finished_pid;

		// check for finished background jobs so they don't turn into zombies
		while ((finished_pid = waitpid(-1, &status, WNOHANG)) > 0) {
			for (auto it = active_jobs.begin(); it != active_jobs.end();) {
				if (it->pid == finished_pid) {
					std::cout << "[" << it->id << "]+ Done " << it->command
							  << std::endl;
					it = active_jobs.erase(it);
					active_jobs.erase(it);
				} else {
					++it;
				}
			}
		}

		std::cout << "$ ";
		std::string input;
		if (!std::getline(std::cin, input))
			break; // user hit ctrl-d or something
		if (input.empty())
			continue;

		auto args = chop_it(input);
		if (args.empty())
			continue;
		std::string cmd = args[0];

		// check if they appended & to run it in the background
		// we check the last token specifically
		bool is_background = false;
		if (args.back() == "&") {
			is_background = true;
			args.pop_back();

			// clean up the input string for display purposes by finding the
			// LAST actual & that was treated as a token (not inside quotes)
			size_t last_amp = input.find_last_of('&');
			if (last_amp != std::string::npos) {
				std::string test_input = input.substr(0, last_amp);
				// double check if removing this & still gives us the same args
				if (chop_it(test_input) == args) {
					input = test_input;
					size_t last = input.find_last_not_of(" \t");
					if (last != std::string::npos) {
						input = input.substr(0, last + 1);
					}
				}
			}
		}

		if (cmd == "exit")
			return 0;

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
			// handle ~ for home directory
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
			for (const auto &job : active_jobs) {
				std::cout << "[" << job.id << "] " << job.pid << " "
						  << job.command << std::endl;
			}
			continue;
		}

		// check if it's an external program
		std::string p = find_it(cmd);
		if (!p.empty()) {
			pid_t pid = fork();
			if (pid == 0) {
				// child process: prepare args and run the thing
				std::vector<char *> c_args;
				for (auto &a : args)
					c_args.push_back(a.data());
				c_args.push_back(nullptr);
				execv(p.c_str(), c_args.data());
				exit(1);
			} else if (is_background) {
				// parent: if bg, just save info and move on
				active_jobs.push_back({next_job_id, pid, input});
				std::cout << "[" << next_job_id << "] " << pid << std::endl;
				next_job_id++;
			} else {
				// parent: wait for it to finish if it's a foreground task
				waitpid(pid, nullptr, 0);
			}
		} else {
			std::cout << cmd << ": command not found" << std::endl;
		}
	}
	return 0;
}
