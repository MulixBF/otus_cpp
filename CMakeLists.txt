cmake_minimum_required (VERSION 3.9.2)
project (allocator)

if(MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    set(BUILD_SHARED_LIBS TRUE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${PROJECT_SOURCE_DIR}/include)

# Library
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_library(allocator ${SRC_FILES})

# Main
add_executable(main ${PROJECT_SOURCE_DIR}/main.cpp)
set_target_properties(main PROPERTIES COMPILE_DEFINITIONS BUILDER_STATIC_DEFINE)
target_link_libraries(main allocator)

# Tests
add_subdirectory(./googletest)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/test/*.cpp)
message(STATUS "Found tests ${TEST_SRC_FILES}")

add_executable(tests ${TEST_SRC_FILES})
target_link_libraries(tests gtest gtest_main)
target_link_libraries(tests allocator)
add_test(UnitTests tests)
