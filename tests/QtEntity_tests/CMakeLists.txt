set(LIB_NAME qtentity_tests)
include_directories(
  ${CMAKE_SOURCE_DIR}/include/
  ${CMAKE_BINARY_DIR}/source
)

set(QTENTITY_TESTS_HDR
    test_entitysystem.h
    test_entitymanager.h
    test_pooledentitysystem.h
    test_prefabsystem.h
    test_readerwriterjson.h
)

set(QTENTITY_TESTS_SRC
    main.cpp
)

QT5_WRAP_CPP(MOC_SOURCES ${QTENTITY_TESTS_HDR})

add_executable(${LIB_NAME} ${QTENTITY_TESTS_HDR} ${QTENTITY_TESTS_SRC} ${MOC_SOURCES})
target_link_libraries(${LIB_NAME} QtEntity)
add_test(NAME ${LIB_NAME} COMMAND ${LIB_NAME} )
qt5_use_modules(${LIB_NAME} Test)

#execute unit tests after each compile
#add_custom_command(TARGET QtEntity POST_BUILD COMMAND ${LIB_NAME})
