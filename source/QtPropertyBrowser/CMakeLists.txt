set(LIB_NAME QtPropertyBrowser)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include/QtPropertyBrowser
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}/../
)
set(CMAKE_AUTOMOC TRUE)
set(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set(LIB_PUBLIC_HEADERS
    ${HEADER_PATH}/qtbuttonpropertybrowser.h
    ${HEADER_PATH}/qteditorfactory.h
    ${HEADER_PATH}/qtgroupboxpropertybrowser.h
    ${HEADER_PATH}/qtpropertybrowser.h
    ${HEADER_PATH}/qtpropertybrowserutils_p.h
    ${HEADER_PATH}/qtpropertymanager.h
    ${HEADER_PATH}/qttreepropertybrowser.h
    ${HEADER_PATH}/qtvariantproperty.h
)

set(LIB_SOURCES
  ${SOURCE_PATH}/qtbuttonpropertybrowser.cpp
  ${SOURCE_PATH}/qteditorfactory.cpp
  ${SOURCE_PATH}/qtgroupboxpropertybrowser.cpp
  ${SOURCE_PATH}/qtpropertybrowser.cpp
  ${SOURCE_PATH}/qtpropertybrowserutils.cpp
  ${SOURCE_PATH}/qtpropertymanager.cpp
  ${SOURCE_PATH}/qttreepropertybrowser.cpp
  ${SOURCE_PATH}/qtvariantproperty.cpp

)

set(MOC_INPUT
  ${HEADER_PATH}/qtbuttonpropertybrowser.h
  ${HEADER_PATH}/qteditorfactory.h
  ${HEADER_PATH}/qtgroupboxpropertybrowser.h
  ${HEADER_PATH}/qtpropertybrowser.h
  ${HEADER_PATH}/qtpropertybrowserutils_p.h
  ${HEADER_PATH}/qtpropertymanager.h
  ${HEADER_PATH}/qttreepropertybrowser.h
  ${HEADER_PATH}/qtvariantproperty.h

)


QT5_ADD_RESOURCES(RCC_SOURCES
   qtpropertybrowser.qrc
)

source_group("Header Files" FILES ${LIB_PUBLIC_HEADERS})

#always link static because no exports defined
add_library(${LIB_NAME} STATIC ${LIB_PUBLIC_HEADERS} ${LIB_SOURCES})

#widgets is needed for QColor and other data types:
qt5_use_modules(${LIB_NAME} Core Widgets)

# generate export macro file in build folder
include (GenerateExportHeader)
generate_export_header(${LIB_NAME}
  EXPORT_FILE_NAME Export
)

include(ModuleInstall OPTIONAL)
