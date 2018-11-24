# @copyright Nils Deppe 2018
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at
# http://boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.3.2)

project(Nebugger VERSION 0.0.0 LANGUAGES CXX C)

set(CMAKE_VERBOSE_MAKEFILE OFF)

site_name(HOSTNAME)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set include directories
include_directories(${CMAKE_SOURCE_DIR}/Linenoise)
include_directories(${CMAKE_SOURCE_DIR})

# Define debugger library sources, including linenoise
set(LIBRARY DEBUGGER_LIB)

set(LIBRARY_SOURCES
  Debugger.cpp
  Linenoise/linenoise.c
  )

add_library(
  ${LIBRARY}
  ${LIBRARY_SOURCES}
  )

# Define executable target
set(EXECUTABLE ndbg)
add_executable(
  ${EXECUTABLE}
  Main.cpp
  )

target_link_libraries(
  ${EXECUTABLE}
  DEBUGGER_LIB
  )
