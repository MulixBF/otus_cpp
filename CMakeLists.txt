cmake_minimum_required (VERSION 3.9.2)
project (otus_cpp_homework_1)


add_compile_definitions(BUILD_NUMBER=${BUILD_NUMBER})
add_executable(helloworld helloworld.cpp)
