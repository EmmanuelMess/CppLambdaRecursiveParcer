cmake_minimum_required(VERSION 3.20)
project(CppParser)
project(CppParserTest)

set(CMAKE_CXX_STANDARD 20)
add_subdirectory("rapidcheck")

add_executable(CppParser main.cpp parser.cpp parser.hpp)

add_executable(CppParserTest test.cpp parser.cpp)
target_link_libraries(CppParserTest rapidcheck)

