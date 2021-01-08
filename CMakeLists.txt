cmake_minimum_required (VERSION 3.9.2)
project (ip_filter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(ip_filter ip_filter.cpp)
