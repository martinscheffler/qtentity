set(LIB_NAME QtEntityUtils)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include/
  ${CMAKE_CURRENT_BINARY_DIR}/.. # for moc files
  ${CMAKE_CURRENT_BINARY_DIR} # for .ui files
  ${CMAKE_CURRENT_SOURCE_DIR}/../QtPropertyBrowser
)

set(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../include/QtEntityUtils)
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set(LIB_PUBLIC_HEADERS
  ${HEADER_PATH}/EntityEditor
  ${HEADER_PATH}/FileEdit
  ${HEADER_PATH}/ListEdit
  ${HEADER_PATH}/PrefabSystem
  ${HEADER_PATH}/VariantFactory
  ${HEADER_PATH}/VariantManager
)

set(LIB_SOURCES
  ${SOURCE_PATH}/EntityEditor.cpp
  ${SOURCE_PATH}/FileEdit.cpp
  ${SOURCE_PATH}/ListEdit.cpp
  ${SOURCE_PATH}/PrefabSystem.cpp
  ${SOURCE_PATH}/VariantFactory.cpp
  ${SOURCE_PATH}/VariantManager.cpp
)

set(MOC_INPUT
  ${HEADER_PATH}/EntityEditor
  ${HEADER_PATH}/FileEdit
  ${HEADER_PATH}/ListEdit
  ${HEADER_PATH}/PrefabSystem
  ${HEADER_PATH}/VariantFactory
  ${HEADER_PATH}/VariantManager
)


QT5_WRAP_CPP(MOC_SOURCES ${MOC_INPUT})

source_group("Header Files" FILES ${LIB_PUBLIC_HEADERS})

add_library(${LIB_NAME} ${LIB_PUBLIC_HEADERS} ${LIB_SOURCES} ${MOC_SOURCES})

target_link_libraries(${LIB_NAME} QtEntity QtPropertyBrowser)

qt5_use_modules(${LIB_NAME} Core Widgets)

# generate export macro file in build folder
include (GenerateExportHeader)
generate_export_header(${LIB_NAME}
  EXPORT_FILE_NAME Export
)

include(ModuleInstall OPTIONAL)
