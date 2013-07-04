cmake_minimum_required(VERSION 2.8.11)
cmake_policy(VERSION 2.8.11)
project(QtEntity)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules")
ENABLE_TESTING()

IF (WIN32)
  SET(CMAKE_DEBUG_POSTFIX  "d")
ENDIF (WIN32)

if(UNIX AND NOT WIN32)
  SET(CMAKE_CXX_FLAGS "-std=c++11")

  # define _DEBUG as set on debug builds on linux
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
  SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")
endif(UNIX AND NOT WIN32)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

find_package(Qt5Widgets REQUIRED)

add_subdirectory(source)
add_subdirectory(tests)
add_subdirectory(examples)
