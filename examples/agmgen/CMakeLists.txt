set(APPNAME agmgen)
set(SOURCES
    agmgen.cpp
    ${SNAPADV_INCLUDE_DIR}/agm.h
    ${SNAPADV_INCLUDE_DIR}/agmfit.h
    ${SNAPADV_SOURCE_DIR}/agm.cpp
    ${SNAPADV_SOURCE_DIR}/agmfit.cpp
)

add_snap_example(${APPNAME} SOURCES)
