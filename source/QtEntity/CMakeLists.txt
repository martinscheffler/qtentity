set(LIB_NAME QtEntity)

include_directories(
  ${CMAKE_SOURCE_DIR}/include/
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}/../
)

set(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../include/QtEntity)
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set(LIB_PUBLIC_HEADERS
  ${HEADER_PATH}/DataTypes
  ${HEADER_PATH}/EntityManager
  ${HEADER_PATH}/EntitySystem
  ${HEADER_PATH}/MetaObjectRegistry
  ${HEADER_PATH}/PooledEntitySystem
  ${HEADER_PATH}/PrefabSystem
  ${HEADER_PATH}/ReaderWriterJSON
  ${HEADER_PATH}/SimpleEntitySystem
)

set(LIB_SOURCES
  ${SOURCE_PATH}/EntityManager.cpp
  ${SOURCE_PATH}/MetaObjectRegistry.cpp
  ${SOURCE_PATH}/PrefabSystem.cpp
  ${SOURCE_PATH}/ReaderWriterJSON.cpp
)

add_library(${LIB_NAME} SHARED ${LIB_PUBLIC_HEADERS} ${LIB_SOURCES} )

#widgets is needed for QColor and other data types:
qt5_use_modules(${LIB_NAME} Core Widgets)

# generate export macro file in build folder
include (GenerateExportHeader)
generate_export_header(${LIB_NAME}
  EXPORT_FILE_NAME Export
)

include(ModuleInstall)

