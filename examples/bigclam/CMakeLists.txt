set(SOURCES
    bigclam.cpp
    ${SNAPADV_SOURCE_DIR}/agm.h
    ${SNAPADV_SOURCE_DIR}/agmfit.h
    ${SNAPADV_SOURCE_DIR}/agmfast.h
    ${SNAPADV_SOURCE_DIR}/agm.cpp
    ${SNAPADV_SOURCE_DIR}/agmfit.cpp
    ${SNAPADV_SOURCE_DIR}/agmfast.cpp
)

add_snap_example(bigclam SOURCES)

target_compile_options(bigclam
    PUBLIC -fopenmp
)
