set(SOURCES
    node2vec.cpp
    ${SNAPADV_SOURCE_DIR}/n2v.h
    ${SNAPADV_SOURCE_DIR}/word2vec.h
    ${SNAPADV_SOURCE_DIR}/biasedrandomwalk.h
    ${SNAPADV_SOURCE_DIR}/n2v.cpp
    ${SNAPADV_SOURCE_DIR}/word2vec.cpp
    ${SNAPADV_SOURCE_DIR}/biasedrandomwalk.cpp
)

add_snap_example(node2vec SOURCES)

target_compile_options(node2vec
    PUBLIC -fopenmp
)
