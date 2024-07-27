cmake_minimum_required(VERSION 3.10)
project(xspeedhack)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(injector cpp/injector.cpp)