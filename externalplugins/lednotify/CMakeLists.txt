project( lednotify )

cmake_minimum_required( VERSION 2.8 )
set( QT_MIN_VERSION "4.7.0" )
set( CMAKE_MIN_VERSION "2.8.0" )

set( KADU_FIND_REQUIRED true )
include( FindKadu )

include_directories( ${KADU_INCLUDE_DIR} )
include_directories( ${KADU_INCLUDE_DIR}/kadu-core )

set( SOURCES
	ledblinker.cpp
	lednotify.cpp
)

set( MOC_SOURCES
	ledblinker.h
	lednotify.h
)

if( WIN32 )
	list( APPEND SOURCES leddriver_win32.cpp )
	include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
else( WIN32 )
	list( APPEND SOURCES leddriver_x11.cpp )
endif( WIN32 )

set( TRANSLATION_SOURCES
	translations/lednotify_en.ts
	translations/lednotify_cs.ts
	translations/lednotify_pl.ts
)

set( CONFIGURATION_FILES
	configuration/lednotify.ui
)

kadu_plugin( lednotify
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
