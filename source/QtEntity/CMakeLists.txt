set(LIBRARY_NAME qtentity)

include_directories(
  ${CMAKE_SOURCE_DIR}/include/
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../include/QtEntity)
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set(LIB_PUBLIC_HEADERS
  ${HEADER_PATH}/DataTypes
  ${HEADER_PATH}/EntityManager
  ${HEADER_PATH}/EntitySystem
  ${HEADER_PATH}/MetaDataSystem
  ${HEADER_PATH}/ReaderWriterJSON
)

set(LIB_SOURCES
  ${SOURCE_PATH}/EntityManager.cpp
  ${SOURCE_PATH}/EntitySystem.cpp
  ${SOURCE_PATH}/MetaDataSystem.cpp
  ${SOURCE_PATH}/ReaderWriterJSON.cpp
)

set(MOC_INPUT
   ${HEADER_PATH}/EntitySystem
   ${HEADER_PATH}/MetaDataSystem
)

QT5_WRAP_CPP(MOC_SOURCES ${MOC_INPUT})

add_library(${LIBRARY_NAME} SHARED ${LIB_PUBLIC_HEADERS} ${LIB_SOURCES} ${MOC_SOURCES})

qt5_use_modules(${LIBRARY_NAME} Widgets)

# generate export macro file in build folder
include (GenerateExportHeader)
generate_export_header(${LIBRARY_NAME}
  EXPORT_FILE_NAME QtEntity/Export
)

