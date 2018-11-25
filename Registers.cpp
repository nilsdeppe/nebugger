/*!
 * @copyright Nils Deppe 2018
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE.md or copy at
 * http://boost.org/LICENSE_1_0.txt)
 */

#include "Registers.hpp"

#include <algorithm>
#include <iostream>
#include <ostream>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>

namespace nebugger {
namespace {
template <class T>
decltype(auto) map_register_to_sys(user_regs_struct& regs_struct,
                                   const Register reg, const T invokable) {
  switch (reg) {
    case Register::rax:
      return invokable(regs_struct.rax);
    case Register::rdx:
      return invokable(regs_struct.rdx);
    case Register::rcx:
      return invokable(regs_struct.rcx);
    case Register::rbx:
      return invokable(regs_struct.rbx);
    case Register::rsi:
      return invokable(regs_struct.rsi);
    case Register::rdi:
      return invokable(regs_struct.rdi);
    case Register::rbp:
      return invokable(regs_struct.rbp);
    case Register::rsp:
      return invokable(regs_struct.rsp);
    case Register::r8:
      return invokable(regs_struct.r8);
    case Register::r9:
      return invokable(regs_struct.r9);
    case Register::r10:
      return invokable(regs_struct.r10);
    case Register::r11:
      return invokable(regs_struct.r11);
    case Register::r12:
      return invokable(regs_struct.r12);
    case Register::r13:
      return invokable(regs_struct.r13);
    case Register::r14:
      return invokable(regs_struct.r14);
    case Register::r15:
      return invokable(regs_struct.r15);
    case Register::rflags:
      return invokable(regs_struct.eflags);
    case Register::es:
      return invokable(regs_struct.es);
    case Register::cs:
      return invokable(regs_struct.cs);
    case Register::ss:
      return invokable(regs_struct.ss);
    case Register::ds:
      return invokable(regs_struct.ds);
    case Register::fs:
      return invokable(regs_struct.fs);
    case Register::gs:
      return invokable(regs_struct.gs);
    case Register::fs_base:
      return invokable(regs_struct.fs_base);
    case Register::gs_base:
      return invokable(regs_struct.gs_base);
    case Register::rip:
      return invokable(regs_struct.rip);
    case Register::orig_rax:
      return invokable(regs_struct.orig_rax);
    default:
      std::cerr << "Unknown register: " << reg << "\n";
      abort();
  };
}
}  // namespace

std::ostream& operator<<(std::ostream& os, const Register reg) {
  return os << std::find_if(register_descriptors.begin(),
                            register_descriptors.end(),
                            [&reg](const auto& t) { return t.reg == reg; })
                   ->name;
}

std::string get_register_name(const Register reg) {
  return std::find_if(register_descriptors.begin(), register_descriptors.end(),
                      [&reg](const auto& t) { return t.reg == reg; })
      ->name;
}

Register get_register_from_name(const std::string& name) {
  const auto it =
      std::find_if(register_descriptors.begin(), register_descriptors.end(),
                   [&name](const auto& t) { return t.name == name; });
  if (it == register_descriptors.end()) {
    throw std::out_of_range(
        "Unknown register during retrieval of register from name " + name);
  }
  return it->reg;
}

int get_dwarf_register(const Register reg) {
  return std::find_if(register_descriptors.begin(), register_descriptors.end(),
                      [&reg](const auto& t) { return t.reg == reg; })
      ->dwarf_reg;
}

Register get_register_from_dwarf_register(const unsigned dwarf_reg) {
  const auto it =
      std::find_if(register_descriptors.begin(), register_descriptors.end(),
                   [&dwarf_reg](const auto& t) {
                     return t.dwarf_reg == static_cast<int>(dwarf_reg);
                   });
  if (it == register_descriptors.end()) {
    throw std::out_of_range(std::string{"Unknown dwarf register"} +
                            std::to_string(dwarf_reg));
  }
  return it->reg;
}

uint64_t get_register_value(const pid_t pid, const Register reg) {
  user_regs_struct regs;
  if (ptrace(PTRACE_GETREGS, pid, nullptr, &regs) == -1) {
    std::cerr << "Failed reading the registers while trying to read: " << reg
              << "\n";
    abort();
  }
  return map_register_to_sys(regs, reg,
                             [](const auto t) -> uint64_t { return t; });
}

void set_register_value(const pid_t pid, const Register reg,
                        const uint64_t value) {
  user_regs_struct regs;
  if (ptrace(PTRACE_GETREGS, pid, nullptr, &regs) == -1) {
    std::cerr << "Failed reading the registers while trying to set: " << reg
              << "\n";
    abort();
  }
  map_register_to_sys(regs, reg, [value](auto& t) { t = value; });
  if (ptrace(PTRACE_SETREGS, pid, nullptr, &regs) == -1) {
    std::cerr << "Failed writing the registers while trying to set: " << reg
              << "\n";
    abort();
  }
}
}  // namespace nebugger
