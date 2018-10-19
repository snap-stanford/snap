cmake_minimum_required(VERSION 2.8)

project(snap)
enable_language(CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC -O3 -DNDEBUG -fopenmp -fmax-errors=1")
elseif(CMAKE_HOST_WIN32)
endif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(glib-core)
add_subdirectory(snap-core)
# add_subdirectory(test)
# add_subdirectory(tutorial)
