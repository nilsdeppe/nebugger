/*!
 * @copyright Nils Deppe 2018
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE.md or copy at
 * http://boost.org/LICENSE_1_0.txt)
 */

#include <iostream>
#include <linenoise.h>
#include <sstream>
#include <string>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#include "Debugger.hpp"

int execute_debugee(const std::string& program_name) {
  // Use ptrace with PTRACE_TRACEME to set the parent process (debugger) to be
  // allowed to trace.
  if (ptrace(PTRACE_TRACEME, 0, nullptr, nullptr) == -1) {
    std::cerr
        << "Failed initialization of tracing on child process with errno: "
        << errno << '\n';
    return -1;
  }
  // Launch the program we were passed as a first argument on the forked
  // process. nullptr is used to terminate the list of arguments passed to the
  // child process.
  if (execl(program_name.c_str(), program_name.c_str(), nullptr) == -1) {
    std::cerr << "Failed launching the program '" << program_name
              << "' in the subprocess with errno: " << errno << '\n';
    return -1;
  }
  return 0;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Program name not specified.\n";
    return -1;
  }

  auto program_name = argv[1];

  // fork() returns 0 in the child process and the PID of the child process on
  // the parent process.
  pid_t pid = fork();
  if (pid == 0) {
    // Debuggee process
    const auto result = execute_debugee(program_name);
    return result;
  } else if (pid >= 1) {
    // Debugger process
    nebugger::Debugger dbg{program_name, pid};
    dbg.run();
  } else {
    std::cerr << "Failed to fork process for debugger.\n";
    return -1;
  }
}
