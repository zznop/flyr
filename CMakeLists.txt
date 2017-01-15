cmake_minimum_required(VERSION 2.8)

project(dudley)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB SOURCE_FILES src/* include/*)

add_executable(dudley ${SOURCE_FILES})

target_link_libraries(dudley -lpthread)
