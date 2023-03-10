set(APPNAME localmotifclustermain)
set(SOURCES
    localmotifclustermain.cpp
    ${SNAPADV_INCLUDE_DIR}/localmotifcluster.h
    ${SNAPADV_SOURCE_DIR}/localmotifcluster.cpp
)

add_snap_example(${APPNAME} SOURCES)

if(CMAKE_HOST_WIN32)
target_compile_options(${APPNAME} PRIVATE /openmp)
else()
target_compile_options(${APPNAME} PRIVATE -fopenmp)
endif(CMAKE_HOST_WIN32)

target_compile_definitions(localmotifclustermain
    PUBLIC F77_POST
)
