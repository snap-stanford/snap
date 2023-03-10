set(glibcore_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/base.h
    PARENT_SCOPE
)

set(glibcore_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/base.cpp
    PARENT_SCOPE
)

# set_target_properties(glibcore PROPERTIES LINKER_LANGUAGE CXX)
# target_link_libraries(glibcore)

set(GLIBCORE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

