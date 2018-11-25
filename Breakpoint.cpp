/*!
 * @copyright Nils Deppe 2018
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE.md or copy at
 * http://boost.org/LICENSE_1_0.txt)
 */

#include "Breakpoint.hpp"

#include <iostream>
#include <sys/ptrace.h>

namespace nebugger {
Breakpoint::Breakpoint(const pid_t pid, const std::intptr_t address)
    : pid_(pid), address_(address) {}

Breakpoint& Breakpoint::enable() {
  // Read/PEEK data at the address from the process
  const long data_at_address = ptrace(PTRACE_PEEKDATA, pid_, address_, nullptr);
  if (data_at_address == -1) {
    std::cerr << "Failed to set breakpoint at address '" << std::hex << address_
              << "' during reading of address.\n";
    return *this;
  }

  // save the bottom 8 bits by doing:
  //   1010010101101011
  // & 0000000011111111
  // ------------------
  //   0000000001101011
  saved_instruction_ = static_cast<uint8_t>(data_at_address & 0xff);

  // The interrupt instruction is 0xcc (8 bits) that we can store in a 64-bit
  // int.
  const uint64_t int3 = 0xcc;
  // swap in the int3 instruction by:
  // 1. getting the top 56 bits by masking ~0xff (complement of 0xff, i.e. flip
  //    all bits)
  // 2. use bitwise or (|) to fill in the lowest 8 bits with the interrupt
  //    instruction.
  const uint64_t data_with_int3 = ((data_at_address & ~0xff) | int3);
  // Inject the new instruction back into the process
  if (ptrace(PTRACE_POKEDATA, pid_, address_, data_with_int3) == -1) {
    std::cerr << "Failed to set breakpoint at address '" << std::hex << address_
              << "' during writing of breakpoint to address.\n";
    return *this;
  }

  enabled_ = true;
  return *this;
}

Breakpoint& Breakpoint::disable() {
  const long data_at_address = ptrace(PTRACE_PEEKDATA, pid_, address_, nullptr);
  if (data_at_address == -1) {
    std::cerr << "Failed to disable breakpoint at address '" << std::hex
              << address_ << "' during reading of address.\n";
    return *this;
  }
  const uint64_t restored_instructions =
      ((data_at_address & ~0xff) | saved_instruction_);
  // Inject the new instruction back into the process
  if (ptrace(PTRACE_POKEDATA, pid_, address_, restored_instructions) == -1) {
    std::cerr << "Failed to disable breakpoint at address '" << std::hex
              << address_ << "' during writing of instructions to address.\n";
    return *this;
  }

  enabled_ = false;
  return *this;
}
}  // namespace nebugger
