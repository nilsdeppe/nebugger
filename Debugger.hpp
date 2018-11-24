/*!
 * @copyright Nils Deppe 2018
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE.md or copy at
 * http://boost.org/LICENSE_1_0.txt)
 */

#pragma once

#include <string>
#include <sys/types.h>
#include <utility>

/// Nils debugger (nebugger) namespace
namespace nebugger {}

namespace nebugger {
class Debugger {
 public:
  Debugger() = delete;
  Debugger(std::string program_name, pid_t pid)
      : program_name_(std::move(program_name)), pid_(pid) {}

  /// Run the debugger waiting on user input.
  void run();

 private:
  void handle_command(const std::string& line);
  void continue_execution();

  std::string program_name_;
  pid_t pid_;
};
}  // namespace nebugger
