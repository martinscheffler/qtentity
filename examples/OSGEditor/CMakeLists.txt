set(APP_NAME OSGEntityEditor)

include_directories(
  ${CMAKE_SOURCE_DIR}/include/
  ${CMAKE_BINARY_DIR}/source/
  ${CMAKE_CURRENT_BINARY_DIR} # for .ui files
  ${OSG_INCLUDE_DIR}
)

FIND_PACKAGE(OpenSceneGraph REQUIRED osgDB osgGA osgUtil osgViewer osgQt osgText osgWidget)

set(HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set(LIB_PUBLIC_HEADERS
  ${HEADER_PATH}/ActorSystem
  ${HEADER_PATH}/Game
  ${HEADER_PATH}/MainWindow  
)

set(LIB_SOURCES
  ${SOURCE_PATH}/ActorSystem.cpp
  ${SOURCE_PATH}/Game.cpp
  ${SOURCE_PATH}/MainWindow.cpp
  ${SOURCE_PATH}/main.cpp  
)

set(MOC_INPUT
   ${HEADER_PATH}/ActorSystem
   ${HEADER_PATH}/Game
   ${HEADER_PATH}/MainWindow   
)

QT5_WRAP_CPP(MOC_SOURCES ${MOC_INPUT})
QT5_WRAP_UI(LIB_SOURCES
        MainWindow.ui
)


add_executable(${APP_NAME} ${LIB_PUBLIC_HEADERS} ${LIB_SOURCES} ${MOC_SOURCES} ${RCC_SOURCES})

target_link_libraries(${APP_NAME} QtEntity QtEntityUtils ${OPENSCENEGRAPH_LIBRARIES})
qt5_use_modules(${APP_NAME} Widgets OpenGL)

