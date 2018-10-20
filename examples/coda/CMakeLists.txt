set(SOURCES
    coda.cpp
    ${SNAPADV_SOURCE_DIR}/agm.h
    ${SNAPADV_SOURCE_DIR}/agmfit.h
    ${SNAPADV_SOURCE_DIR}/agmfast.h
    ${SNAPADV_SOURCE_DIR}/agmdirected.h
    ${SNAPADV_SOURCE_DIR}/agm.cpp
    ${SNAPADV_SOURCE_DIR}/agmfit.cpp
    ${SNAPADV_SOURCE_DIR}/agmfast.cpp
    ${SNAPADV_SOURCE_DIR}/agmdirected.cpp
)

add_snap_example(coda SOURCES)

target_compile_options(coda
    PUBLIC -fopenmp
)
