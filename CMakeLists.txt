cmake_minimum_required(VERSION 3.10)
project(bitchatcopy_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(bitchatcopy_server main.cpp)

target_link_libraries(bitchatcopy_server bluetooth)
