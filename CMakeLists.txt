cmake_minimum_required(VERSION 3.5)

project(test-libsws LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/libsws")

add_executable(test-libsws main.cpp
    test.cpp test.h)

target_include_directories(test-libsws PUBLIC "${CMAKE_CURRENT_LIST_DIR}/libsws")
target_link_libraries(test-libsws "${CMAKE_CURRENT_LIST_DIR}/libsws/libsws.a")

