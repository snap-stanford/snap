# those 2 files include respectively all headers and sources from both glib-core and snap-core.
add_library(snapcore STATIC
    Snap.h
    Snap.cpp
)
set_target_properties(snapcore PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(snapcore SYSTEM PUBLIC ${GLIBCORE_INCLUDE_DIRS})
target_link_libraries(snapcore)


if(CMAKE_HOST_WIN32)
# switches SubSystem from Console to Windows
# and links against windows CRT.
set(WINTAG WIN32)
endif(CMAKE_HOST_WIN32)
################## testSnap
add_executable(testSnap
    ${WINTAG}
    testSnap.cpp
    )
target_include_directories(testSnap SYSTEM PUBLIC ${GLIBCORE_INCLUDE_DIRS})
target_link_libraries(testSnap
    snapcore
    rt
)
