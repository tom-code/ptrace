
cmake_minimum_required(VERSION 3.6)

project(ptrace_test)

set(CMAKE_CXX_FLAGS "-g -Wall -pedantic")

set(CMAKE_CXX_STANDARD 11)

add_custom_command( OUTPUT syscall_names.cc COMMAND python3  gen_syscall_names.py )

add_executable(pt pt.cc syscall_names.cc)

