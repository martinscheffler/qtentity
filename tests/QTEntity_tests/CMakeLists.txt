include_directories(
  ${CMAKE_SOURCE_DIR}/include/
  ${CMAKE_BINARY_DIR}/source/QTEntity
)


set(QTENTITY_TESTS test_entitysystem.cpp)
add_executable(qtentity_tests ${QTENTITY_TESTS})
add_test(NAME qtentity_tests COMMAND qtentity_tests )
qt5_use_modules(qtentity_tests Widgets Test)

