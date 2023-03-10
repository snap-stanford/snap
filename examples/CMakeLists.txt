# meta target that will build ALL examples
add_custom_target(ALL_EXAMPLES ALL)

function(add_snap_example appName sources)
    add_executable(${appName}
        ${${sources}}
        )
    target_include_directories(${appName} SYSTEM
        PUBLIC ${SNAPCORE_INCLUDE_DIRS}
        PUBLIC ${SNAPADV_INCLUDE_DIR}
        PUBLIC ${GLIBCORE_INCLUDE_DIRS}
        PUBLIC ${SNAPEXP_INCLUDE_DIR}
    )
    target_link_libraries(${appName}
        snapcore
        ${${ARGV2}}
    )
    add_dependencies(ALL_EXAMPLES ${appName})
    message("Declared SNAP example ${appName}")
endfunction(add_snap_example)

find_package(arpack QUIET)

add_subdirectory(agmfit)
add_subdirectory(agmgen)
add_subdirectory(bigclam)
add_subdirectory(cascadegen)
add_subdirectory(cascades)
add_subdirectory(centrality)
add_subdirectory(cesna)
add_subdirectory(circles)
add_subdirectory(cliques)
add_subdirectory(coda)
add_subdirectory(community)
add_subdirectory(concomp)
add_subdirectory(flows)
add_subdirectory(forestfire)
add_subdirectory(graphgen)
add_subdirectory(graphhash)
add_subdirectory(infopath)
add_subdirectory(kcores)
add_subdirectory(knnjaccardsim)
add_subdirectory(kronem)
add_subdirectory(kronfit)
add_subdirectory(krongen)
add_subdirectory(localmotifcluster)
add_subdirectory(lshtest)
add_subdirectory(magfit)
add_subdirectory(maggen)
add_subdirectory(mkdatasets)
if(arpack_FOUND)
add_subdirectory(motifcluster)
else()
message("Skipping example 'motifcluster': arpack not found.")
endif(arpack_FOUND)
add_subdirectory(motifs)
add_subdirectory(ncpplot)
add_subdirectory(netevol)
add_subdirectory(netinf)
add_subdirectory(netstat)
add_subdirectory(node2vec)
add_subdirectory(randwalk)
add_subdirectory(rolx)
add_subdirectory(temporalmotifs)
add_subdirectory(testgraph)
