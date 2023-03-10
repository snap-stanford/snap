set(demo_SOURCES
    demo-TUNGraph
    demo-TNGraph
    demo-TNEGraph
    demo-TNodeNet
    demo-TNodeEDatNet
    demo-TNodeEdgeNet
    demo-TNEANet
    demo-TAttr
    demo-TDirNet
    demo-TUndirNet
    demo-multimodal
    demo-subgraph
    demo-ggen
    demo-gio
    demo-alg
    demo-cncom1
    demo-triad
    demo-THash
    demo-topology-benchmark
    demo-TSsParser
)
if(CMAKE_HOST_WIN32)
else()
    set(demo_SOURCES ${test_SOURCES}
        demo-gviz
        demo-bfsdfs
        demo-cncom
        demo-hashvec-benchmark
    )
endif(CMAKE_HOST_WIN32)

# meta target that will build ALL demos
add_custom_target(ALL_TUTORIALS ALL)

foreach(demoFile ${demo_SOURCES})
    string(REPLACE "demo-" "" demoName ${demoFile})
    add_executable(${demoName}
        ${demoFile}.cpp
    )
    target_include_directories(${demoName} SYSTEM
        PUBLIC ${SNAPCORE_INCLUDE_DIRS}
        PUBLIC ${GLIBCORE_INCLUDE_DIRS}
        PRIVATE ${GTEST_INCLUDE_DIRS}
    )
    target_link_libraries(${demoName}
        snapcore
    )
    add_dependencies(ALL_TUTORIALS ${demoName})
    message("Declared SNAP tutorial ${demoName}")
endforeach(demoFile)
