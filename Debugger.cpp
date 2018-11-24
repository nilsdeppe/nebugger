/*!
 * @copyright Nils Deppe 2018
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE.md or copy at
 * http://boost.org/LICENSE_1_0.txt)
 */

#include "Debugger.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#include "Linenoise/linenoise.h"

namespace nebugger {
namespace detail {
std::vector<std::string> split(const std::string& s, char delimiter) {
  std::vector<std::string> out{};
  std::stringstream ss{s};
  std::string item;

  while (std::getline(ss, item, delimiter)) {
    out.push_back(item);
  }

  return out;
}

bool is_prefix(const std::string& s, const std::string& of) {
  if (s.size() > of.size()) {
    return false;
  }
  return std::equal(s.begin(), s.end(), of.begin());
}
}  // namespace detail

void Debugger::run() {
  int wait_status = 0;
  int options = 0;
  if (waitpid(pid_, &wait_status, options) != pid_) {
    std::cerr << "Failed to wait for '" << program_name_
              << "' to launch correctly.\n";
    return;
  }
  char* line = nullptr;
  while ((line = linenoise("dbg> ")) != nullptr) {
    handle_command(line);
    linenoiseHistoryAdd(line);
    linenoiseFree(line);
  }
}

void Debugger::handle_command(const std::string& line) {
  const auto args = detail::split(line, ' ');
  const auto command = args[0];

  // We not just check that command == "continue" or 'c'?
  if (command == "continue" or command == "c") {
    continue_execution();
  } else {
    std::cerr << "Unknown command '" << command << "'.\n";
    return;
  }
}

void Debugger::continue_execution() {
  if (ptrace(PTRACE_CONT, pid_, nullptr, nullptr) == -1) {
    std::cerr << "Failed to continue of tracing on child process with errno: "
              << errno << '\n';
    return;
  }

  int wait_status = 0;
  const int options = 0;
  if (waitpid(pid_, &wait_status, options) != pid_) {
    std::cerr << "Failed to continue process with name '" << program_name_
              << "' correctly.\n";
    return;
  }
}
}  // namespace nebugger
