set(APPNAME cliquesmain)
set(SOURCES
    cliquesmain.cpp
    ${SNAPADV_INCLUDE_DIR}/cliques.h
    ${SNAPADV_SOURCE_DIR}/cliques.cpp
)

add_snap_example(${APPNAME} SOURCES)
