/*!
 * @copyright Nils Deppe 2018
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE.md or copy at
 * http://boost.org/LICENSE_1_0.txt)
 */

#pragma once

#include <cstdint>
#include <string>
#include <sys/types.h>

namespace nebugger {
class Breakpoint {
 public:
  Breakpoint(pid_t pid, std::intptr_t address);

  Breakpoint& enable();
  Breakpoint& disable();

  bool is_enabled() const noexcept { return enabled_; }
  std::intptr_t address() const noexcept { return address_; }

 private:
  pid_t pid_;
  // the address where the breakpoint is located
  std::intptr_t address_;
  bool enabled_{false};
  // The data that used to be at the address we overwrote with 0xcc (int 3) that
  // does the interrupt.
  uint8_t saved_instruction_{0};
};
}  // namespace nebugger
