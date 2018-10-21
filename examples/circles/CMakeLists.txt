set(APPNAME circles)
set(SOURCES
    circles.cpp
    ${SNAPADV_INCLUDE_DIR}/circles.h
)

add_snap_example(${APPNAME} SOURCES)
