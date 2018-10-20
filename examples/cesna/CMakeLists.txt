set(APPNAME cesna)
set(SOURCES
    cesna.cpp
    ${SNAPADV_SOURCE_DIR}/agm.h
    ${SNAPADV_SOURCE_DIR}/agmfit.h
    ${SNAPADV_SOURCE_DIR}/agmfast.h
    ${SNAPADV_SOURCE_DIR}/agmattr.h
    ${SNAPADV_SOURCE_DIR}/agm.cpp
    ${SNAPADV_SOURCE_DIR}/agmfit.cpp
    ${SNAPADV_SOURCE_DIR}/agmfast.cpp
    ${SNAPADV_SOURCE_DIR}/agmattr.cpp
)

add_snap_example(${APPNAME} SOURCES)

if(CMAKE_HOST_WIN32)
target_compile_options(${APPNAME} PRIVATE /openmp)
else()
target_compile_options(${APPNAME} PRIVATE -fopenmp)
endif(CMAKE_HOST_WIN32)
