# Author - Jan Christoph Uhde <Jan@UhdeJC.com> 2015

cmake_minimum_required(VERSION 3.0.0)
project(libsnap VERSION 2.4)

if(CMAKE_HOST_UNIX)
    if(CONFIG STREQUAL "Debug")
        add_compile_options(-Wall -O0 -ggdb)
    else()
        add_compile_options(-fopenmp -O3)
        add_definitions(-DNDEBUG)
    endif()
endif()

## lib
add_library(snap ../snap-core/Snap.cpp)
target_include_directories(snap PUBLIC ../glib-core ../snap-core)

## tutorials
if(TUTORIALS)
    file(GLOB TUTORIAL_CPP ../tutorials/*cpp)
    foreach(tutorial_src ${TUTORIAL_CPP})
        get_filename_component(tutorial ${tutorial_src} NAME_WE)
        add_executable(${tutorial} ${tutorial_src})
        target_link_libraries(${tutorial} snap)
    endforeach()
endif()

## tests
if(TESTS)
    file(GLOB TEST_SRC ../test/*cpp)
    find_package(GTest)
    if(GTEST_FOUND)
        enable_testing()
        add_executable(run-all-tests EXCLUDE_FROM_ALL ${TEST_SRC})
        target_include_directories(run-all-tests PRIVATE ${GTEST_INCLUDE_DIRS})
        target_link_libraries(run-all-tests snap ${GTEST_BOTH_LIBRARIES})
        add_test(test run-all-tests)
    else()
        if(IS_DIRECTORY /usr/src/gtest)
            #if file exists - try to build form source
            enable_testing()
            add_executable(run-all-tests EXCLUDE_FROM_ALL ${TEST_SRC})
            add_subdirectory(/usr/src/gtest ${CMAKE_CURRENT_BINARY_DIR}/gtest)
            target_link_libraries(run-all-tests snap gtest)
            add_test(test run-all-tests)
        endif()
    endif()
endif()


## examples
# adding the examples is unfortunatly not easy :(
#add_subdirectory(../examples)

# building this as lib is impossible as the files depend on the examples
#file(GLOB SNAP_ADV_CPP snap-adv/*cpp)
#file(GLOB GLIB_ADV_CPP glib-adv/*cpp)
#add_library(snap-adv ${SNAP_ADV_CPP} ${GLIB_ADV_CPP})
#list(REMOVE_ITEM GLIB_ADV_CPP "glib-adv/stemming.cpp")
#target_include_directories(snap-adv PUBLIC glib-adv snap-adv)
