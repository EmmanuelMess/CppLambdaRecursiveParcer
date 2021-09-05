cmake_minimum_required(VERSION 3.20)
project(CppParser)

set(CMAKE_CXX_STANDARD 20)

add_executable(CppParser main.cpp)

add_subdirectory("rapidcheck")
target_link_libraries(CppParser rapidcheck)

