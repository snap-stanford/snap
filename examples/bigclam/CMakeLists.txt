set(APPNAME bigclam)
set(SOURCES
    bigclam.cpp
    ${SNAPADV_INCLUDE_DIR}/agm.h
    ${SNAPADV_INCLUDE_DIR}/agmfit.h
    ${SNAPADV_INCLUDE_DIR}/agmfast.h
    ${SNAPADV_SOURCE_DIR}/agm.cpp
    ${SNAPADV_SOURCE_DIR}/agmfit.cpp
    ${SNAPADV_SOURCE_DIR}/agmfast.cpp
)

add_snap_example(${APPNAME} SOURCES)

if(CMAKE_HOST_WIN32)
target_compile_options(${APPNAME} PRIVATE /openmp)
else()
target_compile_options(${APPNAME} PRIVATE -fopenmp)
endif(CMAKE_HOST_WIN32)
