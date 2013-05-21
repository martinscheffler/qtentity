include_directories(
  ${CMAKE_SOURCE_DIR}/include/
  ${CMAKE_BINARY_DIR}/source/QTEntity
)

set(QTENTITY_TESTS_HDR
    test_entitymanager.h
    test_entitysystem.h
)

set(QTENTITY_TESTS_SRC
    test_entitymanager.cpp
    test_entitysystem.cpp
    main.cpp
)
add_executable(qtentity_tests ${QTENTITY_TESTS_HDR} ${QTENTITY_TESTS_SRC})
target_link_libraries(qtentity_tests qtentity)
add_test(NAME qtentity_tests COMMAND qtentity_tests )
qt5_use_modules(qtentity_tests Test)

