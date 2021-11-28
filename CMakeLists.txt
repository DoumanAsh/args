cmake_minimum_required(VERSION 3.5)
set(MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.cmake/Modules")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${MODULE_DIR})
include(msvc)
include(gcc)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED true)

project(args LANGUAGES CXX)

msvc_remove_warning_level()
msvc_set_flags_if("/W4")
gcc_set_flags_if("-Wall -Wextra")

# header only lib
add_library(args INTERFACE)
target_include_directories(args INTERFACE "src/")

# optional unit tests
option(UNIT_TEST "Build unit tests" OFF)
if (UNIT_TEST)
    enable_testing()
    add_subdirectory("tests/")
endif()
