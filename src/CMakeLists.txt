find_package(BISON)
find_package(FLEX)

file(GLOB_RECURSE IDL_PARSER_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/*.hpp
    ${CMAKE_CURRENT_LIST_DIR}/*.h
)

BISON_TARGET(Parser simple_parser.y ${CMAKE_CURRENT_BINARY_DIR}/simple_parser.cpp COMPILE_FLAGS "--no-lines")
FLEX_TARGET(Scanner simple_tokens.l ${CMAKE_CURRENT_BINARY_DIR}/simple_tokens.cpp COMPILE_FLAGS "-d ")
ADD_FLEX_BISON_DEPENDENCY(Scanner Parser)

add_library(idl-parser STATIC
    simple_tokens.l
    simple_parser.y
    ${IDL_PARSER_SOURCES}
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Scanner_OUTPUTS}
)

target_include_directories(idl-parser
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_link_libraries(idl-parser
    fl
)
