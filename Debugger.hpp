/*!
 * @copyright Nils Deppe 2018
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE.md or copy at
 * http://boost.org/LICENSE_1_0.txt)
 */

#pragma once

#include <string>
#include <sys/types.h>
#include <unordered_map>
#include <utility>

#include "Breakpoint.hpp"

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
  void continue_execution();
  void dump_registers();
  uint64_t get_program_counter();
  void handle_command(const std::string& line);
  uint64_t read_memory(const uint64_t address);
  void set_breakpoint_at_address(std::intptr_t address);
  void set_program_counter(const uint64_t program_counter);
  void step_over_breakpoint();
  void wait_for_signal();
  void write_memory(const uint64_t address, const uint64_t value);

  std::string program_name_;
  pid_t pid_;
  std::unordered_map<std::intptr_t, Breakpoint> breakpoints_;
};
}  // namespace nebugger
