cmake_minimum_required(VERSION 2.8.10)

project(QTEntity)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Qt5Widgets REQUIRED)

add_subdirectory(source)
add_subdirectory(tests)
