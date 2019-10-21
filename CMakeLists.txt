cmake_minimum_required(VERSION 3.5)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE ON)
file(GLOB source_files "*.h" "*.cpp")
set (CMAKE_CXX_FLAGS "-ftemplate-backtrace-limit=0")
project(quimulun LANGUAGES CXX)
add_compile_options(-Wall -Wextra -Werror=return-type -std=c++17)
add_executable(quimulun ${source_files})
