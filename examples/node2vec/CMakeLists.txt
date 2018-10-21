set(APPNAME node2vec)
set(SOURCES
    node2vec.cpp
    ${SNAPADV_INCLUDE_DIR}/n2v.h
    ${SNAPADV_INCLUDE_DIR}/word2vec.h
    ${SNAPADV_INCLUDE_DIR}/biasedrandomwalk.h
    ${SNAPADV_SOURCE_DIR}/n2v.cpp
    ${SNAPADV_SOURCE_DIR}/word2vec.cpp
    ${SNAPADV_SOURCE_DIR}/biasedrandomwalk.cpp
)

add_snap_example(${APPNAME} SOURCES)

if(CMAKE_HOST_WIN32)
target_compile_options(${APPNAME} PRIVATE /openmp)
else()
target_compile_options(${APPNAME} PRIVATE -fopenmp)
endif(CMAKE_HOST_WIN32)
