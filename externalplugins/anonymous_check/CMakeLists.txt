project (anonymous_check)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.8.0")
set (CMAKE_MIN_VERSION "2.8.0")

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	anonymous_check.cpp
	anonymous_check-plugin.cpp
	anonymous_info.cpp
)

kadu_plugin (anonymous_check
	PLUGIN_SOURCES ${SOURCES}
)
