set(APP_NAME QtEntityEditor)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../qtentity/include/
  ${CMAKE_CURRENT_BINARY_DIR}/.. # for moc files
  ${CMAKE_CURRENT_BINARY_DIR} # for .ui files
  ${CMAKE_CURRENT_SOURCE_DIR}/../../source/QtPropertyBrowser
  ${CMAKE_CURRENT_BINARY_DIR}/../../source # for qtentity export header
)

set(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set(LIB_PUBLIC_HEADERS
  ${HEADER_PATH}/AttackSystem
  ${HEADER_PATH}/Game
  ${HEADER_PATH}/MainWindow
  ${HEADER_PATH}/Renderer
  ${HEADER_PATH}/ShapeSystem
)

set(LIB_SOURCES
  ${SOURCE_PATH}/AttackSystem.cpp
  ${SOURCE_PATH}/Game.cpp
  ${SOURCE_PATH}/MainWindow.cpp
  ${SOURCE_PATH}/main.cpp
  ${SOURCE_PATH}/Renderer.cpp
  ${SOURCE_PATH}/ShapeSystem.cpp
  main.qml
  shape.qml
)

set(MOC_INPUT
   ${HEADER_PATH}/AttackSystem
   ${HEADER_PATH}/Game
   ${HEADER_PATH}/MainWindow
   ${HEADER_PATH}/ShapeSystem
)


QT5_WRAP_CPP(MOC_SOURCES ${MOC_INPUT})
QT5_WRAP_UI(LIB_SOURCES
        MainWindow.ui
)

QT5_ADD_RESOURCES(RCC_SOURCES
    resources.qrc
)

source_group("Header Files" FILES ${LIB_PUBLIC_HEADERS})
add_executable(${APP_NAME} ${LIB_PUBLIC_HEADERS} ${LIB_SOURCES} ${MOC_SOURCES} ${RCC_SOURCES})

target_link_libraries(${APP_NAME} QtEntity QtEntityUtils)
qt5_use_modules(${APP_NAME} Widgets Quick)

