# those 2 files include respectively all headers and sources from both glib-core and snap-core.
add_library(snapcore STATIC
    Snap.h
    Snap.cpp
)
set_target_properties(snapcore PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(snapcore SYSTEM PUBLIC ${GLIBCORE_INCLUDE_DIRS})
target_link_libraries(snapcore)

if(CMAKE_HOST_WIN32)
target_compile_options(snapcore
	PRIVATE /bigobj
)
endif(CMAKE_HOST_WIN32)
set(SNAPCORE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

################## testSnap
add_executable(testSnap
    ${WINTAG}
    testSnap.cpp
    )
target_include_directories(testSnap SYSTEM PUBLIC ${GLIBCORE_INCLUDE_DIRS})
target_link_libraries(testSnap
    snapcore
    ${testSnapLibs}
)
