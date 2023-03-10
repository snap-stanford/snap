set(APPNAME lshtest)
set(SOURCES
    lshtest.cpp
    ${SNAPEXP_INCLUDE_DIR}/lsh.h
    ${SNAPEXP_SOURCE_DIR}/lsh.cpp
)

add_snap_example(${APPNAME} SOURCES)

if(CMAKE_HOST_WIN32)
target_compile_options(${APPNAME} PRIVATE /openmp)
else()
target_compile_options(${APPNAME} PRIVATE -fopenmp)
endif(CMAKE_HOST_WIN32)
