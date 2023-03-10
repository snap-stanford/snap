set(APPNAME testrolx)
set(SOURCES
    testrolx.cpp
    ${SNAPADV_INCLUDE_DIR}/rolx.h
    ${SNAPADV_SOURCE_DIR}/rolx.cpp
)

add_snap_example(${APPNAME} SOURCES)

if(CMAKE_HOST_WIN32)
target_compile_options(${APPNAME} PRIVATE /openmp)
else()
target_compile_options(${APPNAME} PRIVATE -fopenmp)
endif(CMAKE_HOST_WIN32)
