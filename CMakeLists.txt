cmake_minimum_required(VERSION 3.20)
project(Chess)

set(CMAKE_CXX_STANDARD 23)
file(GLOB SOURCES "*.cpp")
add_executable(Chess ${SOURCES} main.cpp)

