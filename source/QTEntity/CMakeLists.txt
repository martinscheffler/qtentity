set(LIBRARY_NAME qtentity)

include_directories(
  ${CMAKE_SOURCE_DIR}/include/
  ${CMAKE_SOURCE_DIR}/include/QTComponent
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
  ${SOURCE_PATH}/Component.cpp
#  main.cpp
)

add_library(${LIBRARY_NAME} SHARED ${LIB_PUBLIC_HEADERS} ${LIB_SOURCES})

qt5_use_modules(${LIBRARY_NAME} Widgets)

# generate export macro file in build folder
include (GenerateExportHeader)
generate_export_header(${LIBRARY_NAME}
  EXPORT_FILE_NAME QTEntity/Export
)

