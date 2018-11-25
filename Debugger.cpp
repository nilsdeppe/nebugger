/*!
 * @copyright Nils Deppe 2018
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE.md or copy at
 * http://boost.org/LICENSE_1_0.txt)
 */

#include "Debugger.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#include "Linenoise/linenoise.h"
#include "Registers.hpp"

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
  wait_for_signal();
  char* line = nullptr;
  while ((line = linenoise("dbg> ")) != nullptr) {
    handle_command(line);
    linenoiseHistoryAdd(line);
    linenoiseFree(line);
  }
}

void Debugger::dump_registers() {
  for (const auto& t : register_descriptors) {
    std::cout << t.name << " 0x" << std::setfill('0') << std::setw(16)
              << std::hex << get_register_value(pid_, t.reg) << "\n";
  }
}

void Debugger::continue_execution() {
  step_over_breakpoint();
  if (ptrace(PTRACE_CONT, pid_, nullptr, nullptr) == -1) {
    std::cerr << "Failed to continue of tracing on child process with errno: "
              << errno << '\n';
    return;
  }
  wait_for_signal();
}

uint64_t Debugger::get_program_counter() {
  return get_register_value(pid_, Register::rip);
}

void Debugger::handle_command(const std::string& line) {
  const auto args = detail::split(line, ' ');
  const size_t number_of_args = args.size();
  const auto command = args[0];

  // Help strings
  const std::string help_text_memory{
      "memory usage:\n"
      "  - read ADDRESS (address format 0x...)\n"
      "  - write ADDRESS VALUE (address format 0x..., value format 0x...)\n"};
  const std::string help_text_register{
      "register usage:\n"
      "  - dump\n"
      "  - read ADDRESS (address format 0x...)\n"
      "  - write ADDRESS VALUE (address format 0x..., value format 0x...)\n"};

  // We not just check that command == "continue" or 'c'?
  if (command == "continue" or command == "c") {
    continue_execution();
  } else if (command == "b" or command == "break") {
    // requires the address by written as
    std::string address{args[1], 2};
    set_breakpoint_at_address(std::stol(address, 0, 16));
  } else if (command == "register") {
    if (number_of_args == 2 and args[1] == "dump") {
      dump_registers();
    } else if (number_of_args == 3 and args[1] == "read") {
      std::cout << get_register_value(pid_, get_register_from_name(args[2]))
                << '\n';
    } else if (number_of_args == 4 and args[1] == "write") {
      const std::string val{args[3], 2};
      set_register_value(pid_, get_register_from_name(args[2]),
                         std::stol(val, 0, 16));
    } else {
      std::cerr << help_text_register;
    }
  } else if (command == "memory") {
    if (number_of_args == 3 and args[1] == "read") {
      const std::string addr{args[2], 2};
      std::cout << std::hex << read_memory(std::stol(addr, 0, 16)) << "\n";
    }
    if (number_of_args == 4 and args[1] == "write") {
      const std::string addr{args[2], 2};
      const std::string val{args[3], 2};
      write_memory(std::stol(addr, 0, 16), std::stol(val, 0, 16));
    } else {
      std::cerr << help_text_memory;
    }
  } else {
    std::cerr << "Unknown command '" << command << "'.\n";
    return;
  }
}

uint64_t Debugger::read_memory(const uint64_t address) {
  const auto value = ptrace(PTRACE_PEEKDATA, pid_, address, nullptr);
  if (value == -1) {
    std::cerr << "Failed to read value at address " << std::hex << address
              << '\n';
  }
  return static_cast<uint64_t>(value);
}

void Debugger::set_breakpoint_at_address(const std::intptr_t address) {
  Breakpoint bp{pid_, address};
  breakpoints_.insert({address, std::move(bp.enable())});
  std::cout << "Set breakpoint at address 0x" << std::hex << address << "\n";
}

void Debugger::set_program_counter(const uint64_t program_counter) {
  set_register_value(pid_, Register::rip, program_counter);
}

void Debugger::step_over_breakpoint() {
  const auto possible_breakpoint_location = get_program_counter() - 1;
  if (breakpoints_.count(possible_breakpoint_location) > 0) {
    auto& bp = breakpoints_.at(possible_breakpoint_location);

    if (bp.is_enabled()) {
      set_program_counter(possible_breakpoint_location);
      bp.disable();
      if (ptrace(PTRACE_SINGLESTEP, pid_, nullptr, nullptr) == -1) {
        std::cerr << "";
        return;
      }
      wait_for_signal();
      bp.enable();
    }
  }
}

void Debugger::wait_for_signal() {
  int wait_status = 0;
  const int options = 0;
  if (waitpid(pid_, &wait_status, options) != pid_) {
    std::cerr << "Failed to continue process with name '" << program_name_
              << "' correctly.\n";
  }
}

void Debugger::write_memory(const uint64_t address, const uint64_t value) {
  if (ptrace(PTRACE_POKEDATA, pid_, address, value) == -1) {
    std::cerr << "Failed to write value " << value << " to address " << std::hex
              << address << '\n';
  }
}
}  // namespace nebugger
