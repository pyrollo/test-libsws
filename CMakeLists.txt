cmake_minimum_required(VERSION 3.5)

project(test-libsws LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libsws)

add_executable(test-libsws
    main.cpp
    test.cpp test.h)

target_include_directories(test-libsws PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libsws)
target_link_libraries(test-libsws sws)

