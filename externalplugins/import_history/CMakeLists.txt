project (import_history)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.8.0")
set (CMAKE_MIN_VERSION "2.8.0")

find_package (Kadu REQUIRED CONFIG)

include (FindPkgConfig)

set (SOURCES
  import_history.cpp
  gui/gui.cpp
  importers/importer.cpp
  importers/gg7/crc32.cpp
  importers/gg7/ggimporter.cpp
  importers/gg8/gg8importer.cpp
  misc/memfile.cpp
)

set (CONFIGURATION_FILES
)

pkg_search_module (LIBGADU REQUIRED libgadu)
pkg_search_module (SQLITE3 REQUIRED sqlite3)

include_directories (${LIBGADU_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIRS})
link_directories (${LIBGADU_LIBRARY_DIRS} ${SQLITE3_LIBRARY_DIRS})

#qt5_wrap_ui(import_history_UIS gui/import_history.ui)
include_directories (${CMAKE_CURRENT_BINARY_DIR})

kadu_plugin(import_history
  PLUGIN_SOURCES ${SOURCES} ${import_history_UIS}
  PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
  PLUGIN_LIBRARIES ${LIBGADU_LIBRARIES} ${SQLITE3_LIBRARIES}
  PLUGIN_DEPENDENCIES history gadu_protocol
)
