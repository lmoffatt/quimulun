cmake_minimum_required(VERSION 3.5)

project(quimulun LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
file(GLOB source_files "*.h" "*.cpp")
set (CMAKE_CXX_FLAGS "-ftemplate-backtrace-limit=0")
add_executable(quimulun ${source_files})
