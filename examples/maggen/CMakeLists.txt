set(APPNAME maggen)
set(SOURCES
    maggen.cpp
    ${SNAPADV_INCLUDE_DIR}/mag.h
    ${SNAPADV_SOURCE_DIR}/mag.cpp
)

add_snap_example(${APPNAME} SOURCES)
