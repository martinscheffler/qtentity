cmake_minimum_required(VERSION 2.8.10)

project(QtEntity)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(UNIX AND NOT WIN32)
  SET(CMAKE_CXX_FLAGS "-std=c++11")
endif(UNIX AND NOT WIN32)

find_package(Qt5Widgets REQUIRED)

add_subdirectory(source)
add_subdirectory(tests)
add_subdirectory(examples)
