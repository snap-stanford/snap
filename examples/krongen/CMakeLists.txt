set(APPNAME krongen)
set(SOURCES
    krongen.cpp
    ${SNAPADV_INCLUDE_DIR}/kronecker.h
    ${SNAPADV_SOURCE_DIR}/kronecker.cpp
)

add_snap_example(${APPNAME} SOURCES)
