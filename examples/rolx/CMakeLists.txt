set(SOURCES
    testrolx.cpp
    ${SNAPADV_SOURCE_DIR}/rolx.h
    ${SNAPADV_SOURCE_DIR}/rolx.cpp
)

add_snap_example(testrolx SOURCES)

target_compile_options(testrolx
    PUBLIC -fopenmp
)
