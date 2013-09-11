set(LIB_NAME QtPropertyBrowser)

include_directories(
  ${CMAKE_SOURCE_DIR}/include/
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/source/
)

set(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../include/QtPropertyBrowser)
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set(LIB_PUBLIC_HEADERS
    ${HEADER_PATH}/QtAbstractEditorFactoryBase
    ${HEADER_PATH}/QtAbstractPropertyBrowser
    ${HEADER_PATH}/QtAbstractPropertyManager
    ${HEADER_PATH}/QtBoolPropertyManager
    ${HEADER_PATH}/QtBrowserItem
    ${HEADER_PATH}/QtButtonPropertyBrowser
    ${HEADER_PATH}/qtbuttonpropertybrowser.h
    ${HEADER_PATH}/QtCharEditorFactory
    ${HEADER_PATH}/QtCharPropertyManager
    ${HEADER_PATH}/QtCheckBoxFactory
    ${HEADER_PATH}/QtColorEditorFactory
    ${HEADER_PATH}/QtColorPropertyManager
    ${HEADER_PATH}/QtCursorEditorFactory
    ${HEADER_PATH}/QtCursorPropertyManager
    ${HEADER_PATH}/QtDateEditFactory
    ${HEADER_PATH}/QtDatePropertyManager
    ${HEADER_PATH}/QtDateTimeEditFactory
    ${HEADER_PATH}/QtDateTimePropertyManager
    ${HEADER_PATH}/QtDoublePropertyManager
    ${HEADER_PATH}/QtDoubleSpinBoxFactory
    ${HEADER_PATH}/qteditorfactory.h
    ${HEADER_PATH}/QtEnumEditorFactory
    ${HEADER_PATH}/QtEnumPropertyManager
    ${HEADER_PATH}/QtFlagPropertyManager
    ${HEADER_PATH}/QtFontEditorFactory
    ${HEADER_PATH}/QtFontPropertyManager
    ${HEADER_PATH}/QtGroupBoxPropertyBrowser
    ${HEADER_PATH}/qtgroupboxpropertybrowser.h
    ${HEADER_PATH}/QtGroupPropertyManager
    ${HEADER_PATH}/QtIntPropertyManager
    ${HEADER_PATH}/QtKeySequenceEditorFactory
    ${HEADER_PATH}/QtKeySequencePropertyManager
    ${HEADER_PATH}/QtLineEditFactory
    ${HEADER_PATH}/QtLocalePropertyManager
    ${HEADER_PATH}/QtPointFPropertyManager
    ${HEADER_PATH}/QtPointPropertyManager
    ${HEADER_PATH}/QtProperty
    ${HEADER_PATH}/qtpropertybrowser.h
    ${HEADER_PATH}/qtpropertybrowserutils_p.h
    ${HEADER_PATH}/qtpropertymanager.h
    ${HEADER_PATH}/QtRectFPropertyManager
    ${HEADER_PATH}/QtRectPropertyManager
    ${HEADER_PATH}/QtScrollBarFactory
    ${HEADER_PATH}/QtSizeFPropertyManager
    ${HEADER_PATH}/QtSizePolicyPropertyManager
    ${HEADER_PATH}/QtSizePropertyManager
    ${HEADER_PATH}/QtSliderFactory
    ${HEADER_PATH}/QtSpinBoxFactory
    ${HEADER_PATH}/QtStringPropertyManager
    ${HEADER_PATH}/QtTimeEditFactory
    ${HEADER_PATH}/QtTimePropertyManager
    ${HEADER_PATH}/QtTreePropertyBrowser
    ${HEADER_PATH}/qttreepropertybrowser.h
    ${HEADER_PATH}/QtVariantEditorFactory
    ${HEADER_PATH}/QtVariantProperty
    ${HEADER_PATH}/qtvariantproperty.h
    ${HEADER_PATH}/QtVariantPropertyManager
	${HEADER_PATH}/QtVector3DPropertyManager

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

QT5_WRAP_CPP(MOC_SOURCES ${MOC_INPUT})

QT5_ADD_RESOURCES(RCC_SOURCES
   qtpropertybrowser.qrc
)

add_library(${LIB_NAME} ${LIB_PUBLIC_HEADERS} ${LIB_SOURCES} ${MOC_SOURCES})

#widgets is needed for QColor and other data types:
qt5_use_modules(${LIB_NAME} Core Widgets)

# generate export macro file in build folder
include (GenerateExportHeader)
generate_export_header(${LIB_NAME}
  EXPORT_FILE_NAME Export
)

include(ModuleInstall OPTIONAL)
