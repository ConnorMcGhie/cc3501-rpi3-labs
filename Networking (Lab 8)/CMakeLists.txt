cmake_minimum_required(VERSION 3.12)
project(Networking)
set(CMAKE_CXX_STANDARD 20)
set(COMPILE_DEFINITIONS -O2 -Wall)

add_executable(post_message src/post_message.cpp)
target_link_libraries(post_message curl)
