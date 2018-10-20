set(SOURCES
    maggen.cpp
    ${SNAPADV_SOURCE_DIR}/mag.h
    ${SNAPADV_SOURCE_DIR}/mag.cpp
)

add_snap_example(maggen SOURCES)
