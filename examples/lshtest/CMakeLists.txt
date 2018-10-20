set(SOURCES
    lshtest.cpp
    ${SNAPEXP_SOURCE_DIR}/lsh.h
    ${SNAPEXP_SOURCE_DIR}/lsh.cpp
)

add_snap_example(lshtest SOURCES)
target_compile_options(lshtest
    PUBLIC -fopenmp
)
