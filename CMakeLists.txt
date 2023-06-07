cmake_minimum_required(VERSION 3.16)

set(CFXS_UNITY_BUILD ON)

include("CMake/Toolchain/GCC_ARM.cmake")
project(ImpedanceAnalyzer C CXX)
add_subdirectory(CMake)
add_subdirectory(ImpedanceAnalyzer)
