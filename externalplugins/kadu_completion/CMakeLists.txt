project (kadu_completion)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.8.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
    completion_context.cpp
    kadu_completion.cpp
    kadu_completion_plugin.cpp
)

set (MOC_SOURCES
    completion_context.h
    kadu_completion.h
    kadu_completion_plugin.h
)

set (DATA_FILES
    data/list.txt
)

kadu_plugin (kadu_completion
    PLUGIN_SOURCES ${SOURCES}
    PLUGIN_MOC_SOURCES ${MOC_SOURCES}
)

install (FILES ${DATA_FILES}
    DESTINATION ${KADU_PLUGINS_DIR}/data/kadu_completion)
