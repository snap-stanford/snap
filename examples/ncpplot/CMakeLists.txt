set(APPNAME ncpplot)
set(SOURCES
    ncpplot.cpp
    ${SNAPADV_INCLUDE_DIR}/ncp.h
    ${SNAPADV_SOURCE_DIR}/ncp.cpp
)

add_snap_example(${APPNAME} SOURCES)
