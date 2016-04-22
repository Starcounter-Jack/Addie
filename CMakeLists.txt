cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

project (Addie)

file(GLOB_RECURSE Addie_SOURCES "src/*.cpp")
file(GLOB_RECURSE Addie_HEADERS "src/*.h")

set (Addie_INCLUDE_DIRS "")
foreach (_headerFile ${Addie_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND Addie_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES Addie_INCLUDE_DIRS)

include_directories(${Addie_INCLUDE_DIRS})
add_executable (addie ${Addie_SOURCES})
