cmake_minimum_required(VERSION 3.10)

project(beachat_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(beachat_server 
    src/server/main.cpp
    src/server/Server.cpp)

target_link_libraries(beachat_server bluetooth)
