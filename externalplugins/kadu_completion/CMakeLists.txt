project (kadu_completion)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.8.0")
set (CMAKE_MIN_VERSION "2.8.0")

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
    completion_context.cpp
    kadu_completion.cpp
    kadu_completion_plugin.cpp
)

set (DATA_FILES
    data/list.txt
)

kadu_plugin (kadu_completion
    PLUGIN_SOURCES ${SOURCES}
)

install (FILES ${DATA_FILES}
    DESTINATION ${KADU_INSTALL_PLUGINS_DATA_DIR}/data/kadu_completion)
