set(APPNAME netinf)
set(SOURCES
    netinf.cpp
    ${SNAPADV_INCLUDE_DIR}/cascnetinf.h
    ${SNAPADV_SOURCE_DIR}/cascnetinf.cpp
)

add_snap_example(${APPNAME} SOURCES)
