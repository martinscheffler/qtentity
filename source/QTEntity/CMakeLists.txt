include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include/
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../include/QTEntity)
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set(LIB_PUBLIC_HEADERS
  ${HEADER_PATH}/Component
  ${HEADER_PATH}/DataTypes
  ${HEADER_PATH}/EntityManager
  ${HEADER_PATH}/EntitySystem
)

set(LIB_SOURCES
  main.cpp
)

add_library(qtentity SHARED ${LIB_PUBLIC_HEADERS} ${LIB_SOURCES})

qt5_use_modules(qtentity Widgets Test)
