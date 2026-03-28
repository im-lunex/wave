#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>

namespace fs = std::filesystem;

// split this shit into parts
std::vector<std::string> split(const std::string& s) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (tokenStream >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

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
    if (!std::getline(std::cin, input)) break;
    if (input.empty()) continue;

    std::vector<std::string> args = split(input);
    std::string cmd = args[0];

    if (cmd == "exit") {
      return 0; // gtfo
    } else if (cmd == "echo") {
      for (size_t i = 1; i < args.size(); ++i) {
          std::cout << args[i] << (i == args.size() - 1 ? "" : " ");
      }
      std::cout << std::endl;
    } else if (cmd == "type") {
      if (args.size() < 2) continue;
      std::string arg = args[1];
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
      std::string path = find_path(cmd);
      if (!path.empty()) {
        // holly shit, run it
        pid_t pid = fork();
        if (pid == 0) {
          std::vector<char*> c_args;
          for (auto& a : args) c_args.push_back(&a[0]);
          c_args.push_back(nullptr);
          execv(path.c_str(), c_args.data());
          exit(1); // if execv fails, shit
        } else {
          waitpid(pid, nullptr, 0);
        }
      } else {
        std::cout << cmd << ": command not found" << std::endl;
      }
    }
  }
  
  return 0;
}
