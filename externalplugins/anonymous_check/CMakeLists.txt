project (anonymous_check)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.8.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	anonymous_check.cpp
	anonymous_check-plugin.cpp
	anonymous_info.cpp
)

set (MOC_SOURCES
	anonymous_check-plugin.h
	anonymous_check.h
)

set (TRANSLATION_SOURCES
	translations/anonymous_check_pl.ts
)

kadu_plugin (anonymous_check
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
)

