cmake_minimum_required(VERSION 2.8.9)
project(QTEntity)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)

set(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include/QTEntity)
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/source)

set(LIB_PUBLIC_HEADERS
  ${HEADER_PATH}/Component
  ${HEADER_PATH}/DataTypes
  ${HEADER_PATH}/EntityManager
  ${HEADER_PATH}/EntitySystem
)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

add_library(qtentity SHARED main.cpp ${LIB_PUBLIC_HEADERS})


qt5_use_modules(qtentity Widgets Test)

set(QTENTITY_TESTS ${SOURCE_PATH}/qtentity_tests/test_entitysystem.cpp)
#add_executable(qtentity_tests ${QTENTITY_TESTS})

