set(APPNAME infopath)
set(SOURCES
    infopath.cpp
    ${SNAPADV_SOURCE_DIR}/cascdynetinf.h
    ${SNAPADV_SOURCE_DIR}/cascdynetinf.cpp
)

add_snap_example(${APPNAME} SOURCES)
