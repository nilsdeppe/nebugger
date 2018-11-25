/*!
 * @copyright Nils Deppe 2018
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE.md or copy at
 * http://boost.org/LICENSE_1_0.txt)
 */

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

namespace nebugger {
/// A list of all the registers we support reading from and writing to.
enum class Register : int64_t {
  rax,
  rdx,
  rcx,
  rbx,
  rsi,
  rdi,
  rbp,
  rsp,
  r8,
  r9,
  r10,
  r11,
  r12,
  r13,
  r14,
  r15,
  rflags,
  es,
  cs,
  ss,
  ds,
  fs,
  gs,
  fs_base,
  gs_base,
  rip,
  orig_rax
};

/// Total number of register we can handle
constexpr std::size_t number_of_registers = 27;

/// Mapping between the Register enum, the dwarf register, and the register
/// name.
///
/// Register info at: https://www.uclibc.org/docs/psABI-x86_64.pdf
struct RegisterDescriptor {
  Register reg;
  int dwarf_reg;
  std::string name;
};

const std::array<RegisterDescriptor, number_of_registers> register_descriptors{{
    {Register::r15, 15, "r15"},
    {Register::r14, 14, "r14"},
    {Register::r13, 13, "r13"},
    {Register::r12, 12, "r12"},
    {Register::rbp, 6, "rbp"},
    {Register::rbx, 3, "rbx"},
    {Register::r11, 11, "r11"},
    {Register::r10, 10, "r10"},
    {Register::r9, 9, "r9"},
    {Register::r8, 8, "r8"},
    {Register::rax, 0, "rax"},
    {Register::rcx, 2, "rcx"},
    {Register::rdx, 1, "rdx"},
    {Register::rsi, 4, "rsi"},
    {Register::rdi, 5, "rdi"},
    {Register::orig_rax, -1, "orig_rax"},
    {Register::rip, -1, "rip"},
    {Register::cs, 51, "cs"},
    {Register::rflags, 49, "eflags"},
    {Register::rsp, 7, "rsp"},
    {Register::ss, 52, "ss"},
    {Register::fs_base, 58, "fs_base"},
    {Register::gs_base, 59, "gs_base"},
    {Register::ds, 53, "ds"},
    {Register::es, 50, "es"},
    {Register::fs, 54, "fs"},
    {Register::gs, 55, "gs"},
}};

std::ostream& operator<<(std::ostream& os, const Register reg);

/// Get a string of the name of the Register
std::string get_register_name(const Register reg);

/// Given the name of a Register
Register get_register_from_name(const std::string& name);

int get_dwarf_register(const Register t);

Register get_register_from_dwarf_register(const unsigned dwarf_reg);

uint64_t get_register_value(const pid_t pid, const Register reg);

void set_register_value(const pid_t pid, const Register reg,
                        const uint64_t value);
}  // namespace nebugger
