cmake_minimum_required(VERSION 3.26)
project(PraktikumAlgorithmenWS23_24)

set(CMAKE_CXX_STANDARD 20)

include_directories(src)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(lib/googletest)
