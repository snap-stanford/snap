set(SOURCES
    motifclustermain.cpp
    ${SNAPADV_SOURCE_DIR}/motifcluster.h
    ${SNAPADV_SOURCE_DIR}/motifcluster.cpp
)

set(EXTRA_LIBS arpack)

add_snap_example(motifclustermain SOURCES EXTRA_LIBS)
target_link_libraries(motifclustermain
    PUBLIC
)
target_compile_definitions(motifclustermain
    PUBLIC F77_POST
)
