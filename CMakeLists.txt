cmake_minimum_required(VERSION 3.28)
project(barbariska)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/barbariska)

add_subdirectory(core)
add_subdirectory(bar)
add_subdirectory(daemon)
