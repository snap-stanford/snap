set(APPNAME temporalmotifsmain)
set(SOURCES
    temporalmotifsmain.cpp
    ${SNAPADV_INCLUDE_DIR}/temporalmotifs.h
    ${SNAPADV_SOURCE_DIR}/temporalmotifs.cpp
)

add_snap_example(${APPNAME} SOURCES)
