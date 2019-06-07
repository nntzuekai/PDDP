cmake_minimum_required(VERSION 3.4.3)
project(PDDP)
#find_package(LLVM REQUIRED CONFIG)
#
#message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
#message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
#
#include_directories("/usr/include/llvm-6.0")
#include_directories("/usr/include/llvm-c-6.0")
#add_definitions(${LLVM_DEFINITIONS})

set(CMAKE_CXX_FLAGS "-Wall ")
set(CMAKE_CXX_STANDARD 17)

add_executable(PDDP main.cpp PDDP_tree.h)

#llvm_map_components_to_libnames(llvm_libs support core irreader)

#target_link_libraries(PDDP ${llvm_libs})