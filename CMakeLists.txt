cmake_minimum_required(VERSION 2.8.10)

project(QtEntity)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
SET(CMAKE_CXX_FLAGS "-std=c++11")
find_package(Qt5Widgets REQUIRED)

add_subdirectory(source)
add_subdirectory(tests)
add_subdirectory(examples)
