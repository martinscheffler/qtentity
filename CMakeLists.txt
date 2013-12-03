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

OPTION(BUILD_SHARED_LIBS "Set to ON to build QtEntity for dynamic linking.  Use OFF for static." OFF)
SET(LIB_TYPE_STATIC)
IF(BUILD_SHARED_LIBS)
  SET(LIB_TYPE SHARED)
ENDIF(BUILD_SHARED_LIBS)
IF (WIN32)
  SET(CMAKE_STATIC_LIBRARY_SUFFIX "_static.lib")
ENDIF (WIN32)

message("Building shared library: " ${QTENTITY_LIBRARY_SHARED})
find_package(Qt5Widgets REQUIRED)

add_subdirectory(source)
add_subdirectory(tests)
add_subdirectory(examples)
