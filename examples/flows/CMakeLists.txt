set(SOURCES
    flows.cpp
)

add_snap_example(flows SOURCES)

target_compile_options(flows
    PUBLIC -fopenmp
)
