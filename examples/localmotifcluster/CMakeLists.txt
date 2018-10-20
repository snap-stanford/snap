set(SOURCES
    localmotifclustermain.cpp
    ${SNAPADV_SOURCE_DIR}/localmotifcluster.h
    ${SNAPADV_SOURCE_DIR}/localmotifcluster.cpp
)

add_snap_example(localmotifclustermain SOURCES)
target_compile_options(localmotifclustermain
    PUBLIC -fopenmp
)
target_compile_definitions(localmotifclustermain
    PUBLIC F77_POST
)
