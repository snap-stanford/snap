set(demoFiles
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
    demo-gviz
    demo-alg
    demo-cncom
    demo-cncom1
    demo-bfsdfs
    demo-triad
    demo-THash
    demo-topology-benchmark
    demo-hashvec-benchmark
    demo-TSsParser
)

# meta target that will build ALL demos
add_custom_target(tutorials ALL)

foreach(demoFile ${demoFiles})
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
    add_dependencies(tutorials ${demoName})
    message("Declared SNAP tutorial ${demoName}")
endforeach(demoName)
