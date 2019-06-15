find_package(GTest REQUIRED)

file(GLOB_RECURSE IDL_PARSER_TEST_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/*.hpp
    ${CMAKE_CURRENT_LIST_DIR}/*.h
)

add_executable(
    idl-parser-tests
    ${IDL_PARSER_TEST_SOURCES}
)

target_link_libraries(idl-parser-tests
    idl-parser
    GTest::GTest GTest::Main
)
