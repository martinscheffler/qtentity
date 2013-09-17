set(APP_NAME QtEntityEditor)

include_directories(
  ${CMAKE_SOURCE_DIR}/include/
  ${CMAKE_BINARY_DIR}/source/
  ${CMAKE_CURRENT_BINARY_DIR} # for .ui files
)

set(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set(LIB_PUBLIC_HEADERS
  ${HEADER_PATH}/BulletSystem
  ${HEADER_PATH}/DamageSystem
  ${HEADER_PATH}/EnemySystem
  ${HEADER_PATH}/Game
  ${HEADER_PATH}/MainWindow
  ${HEADER_PATH}/MetaDataSystem
  ${HEADER_PATH}/Renderer
  ${HEADER_PATH}/ShapeSystem
)

set(LIB_SOURCES
  ${SOURCE_PATH}/BulletSystem.cpp
  ${SOURCE_PATH}/DamageSystem.cpp
  ${SOURCE_PATH}/EnemySystem.cpp
  ${SOURCE_PATH}/Game.cpp
  ${SOURCE_PATH}/MainWindow.cpp
  ${SOURCE_PATH}/main.cpp
  ${HEADER_PATH}/MetaDataSystem.cpp
  ${SOURCE_PATH}/Renderer.cpp
  ${SOURCE_PATH}/ShapeSystem.cpp
)

set(MOC_INPUT
   ${HEADER_PATH}/Game
   ${HEADER_PATH}/MainWindow
   ${HEADER_PATH}/MetaDataSystem
)

QT5_WRAP_CPP(MOC_SOURCES ${MOC_INPUT})
QT5_WRAP_UI(LIB_SOURCES
        MainWindow.ui
)

QT5_ADD_RESOURCES(RCC_SOURCES
    resources.qrc
)

add_executable(${APP_NAME} ${LIB_PUBLIC_HEADERS} ${LIB_SOURCES} ${MOC_SOURCES} ${RCC_SOURCES})

target_link_libraries(${APP_NAME} QtEntity QtEntityUtils)
qt5_use_modules(${APP_NAME} Widgets)

