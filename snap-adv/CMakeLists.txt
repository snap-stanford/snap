set(SNAPADV_HEADERS
    agm.h
    agmattr.h
    agmdirected.h
    agmfast.h
    agmfit.h
    biasedrandomwalk.h
    cascdynetinf.h
    cascnetinf.h
    circles.h
    cliques.h
    CMakeLists.txt
    graphcounter.h
    kronecker.h
    localmotifcluster.h
    mag.h
    motifcluster.h
    n2v.h
    ncp.h
    rolx.h
    subgraphenum.h
    temporalmotifs.h
    word2vec.h
)
set(SNAPADV_SOURCES
    agm.cpp
    agmattr.cpp
    agmdirected.cpp
    agmfast.cpp
    agmfit.cpp
    biasedrandomwalk.cpp
    cascdynetinf.cpp
    cascnetinf.cpp
    cliques.cpp
    graphcounter.cpp
    kronecker.cpp
    localmotifcluster.cpp
    mag.cpp
    motifcluster.cpp
    n2v.cpp
    ncp.cpp
    rolx.cpp
    subgraphenum.cpp
    temporalmotifs.cpp
    word2vec.cpp
)

add_library(snapadv STATIC
    ${SNAPADV_HEADERS}
    ${SNAPADV_SOURCES}
)
set_target_properties(snapadv PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(snapadv SYSTEM
    PUBLIC ${SNAPCORE_INCLUDE_DIRS}
    PUBLIC ${GLIBCORE_INCLUDE_DIRS}
)
target_link_libraries(snapadv snapcore)

if(CMAKE_HOST_WIN32)
target_compile_options(snapadv
	PRIVATE /bigobj
)
endif(CMAKE_HOST_WIN32)
set(SNAPADV_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

