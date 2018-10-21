set(APPNAME motifclustermain)
set(SOURCES
    motifclustermain.cpp
    ${SNAPADV_INCLUDE_DIR}/motifcluster.h
    ${SNAPADV_SOURCE_DIR}/motifcluster.cpp
)

set(EXTRA_LIBS arpack)

add_snap_example(${APPNAME} SOURCES)
target_compile_definitions(${APPNAME}
    PUBLIC F77_POST
)
