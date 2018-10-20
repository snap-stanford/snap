set(APPNAME circles)
set(SOURCES
    circles.cpp
    ${SNAPADV_SOURCE_DIR}/circles.h
)

add_snap_example(${APPNAME} SOURCES)
