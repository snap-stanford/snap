set(SOURCES
    agmgen.cpp
    ${SNAPADV_SOURCE_DIR}/agm.h
    ${SNAPADV_SOURCE_DIR}/agmfit.h
    ${SNAPADV_SOURCE_DIR}/agm.cpp
    ${SNAPADV_SOURCE_DIR}/agmfit.cpp
)

add_snap_example(agmgen SOURCES)
