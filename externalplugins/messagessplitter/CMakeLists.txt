project( messagessplitter )

cmake_minimum_required( VERSION 2.8 )
set( QT_MIN_VERSION "4.7.0" )
set( CMAKE_MIN_VERSION "2.8.0" )

set( KADU_FIND_REQUIRED true )
include( FindKadu )

include_directories( ${KADU_INCLUDE_DIR} )
include_directories( ${KADU_INCLUDE_DIR}/kadu-core )

set( SOURCES
	messagessplitter.cpp
)

set( MOC_SOURCES
	messagessplitter.h
)

set( TRANSLATION_SOURCES
	translations/messagessplitter_en.ts
	translations/messagessplitter_cs.ts
	translations/messagessplitter_pl.ts
)

set( CONFIGURATION_FILES
	configuration/messagessplitter.ui
)

kadu_plugin( messagessplitter
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES gadu_protocol encryption_ng
)
