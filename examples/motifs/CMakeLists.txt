set(SOURCES
    motifs.cpp
    ${SNAPADV_SOURCE_DIR}/subgraphenum.h
    ${SNAPADV_SOURCE_DIR}/graphcounter.h
    ${SNAPADV_SOURCE_DIR}/subgraphenum.cpp
    ${SNAPADV_SOURCE_DIR}/graphcounter.cpp
)

add_snap_example(motifs SOURCES)
