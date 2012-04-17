project( networkping )

cmake_minimum_required( VERSION 2.8 )
set( QT_MIN_VERSION "4.7.0" )
set( CMAKE_MIN_VERSION "2.8.0" )

set( KADU_FIND_REQUIRED true )
include( FindKadu )

include_directories( ${KADU_INCLUDE_DIR} )
include_directories( ${KADU_INCLUDE_DIR}/kadu-core )

set( SOURCES
	networkping.cpp
)

set( MOC_SOURCES
	networkping.h
)

set( TRANSLATION_SOURCES
	translations/networkping_en.ts
	translations/networkping_pl.ts
)

set( CONFIGURATION_FILES
	configuration/networkping.ui
)

kadu_plugin( networkping
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
