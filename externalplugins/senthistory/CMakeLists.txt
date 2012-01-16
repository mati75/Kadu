project( senthistory )

cmake_minimum_required( VERSION 2.8 )
set( QT_MIN_VERSION "4.7.0" )
set( CMAKE_MIN_VERSION "2.8.0" )

set( KADU_FIND_REQUIRED true )
include( FindKadu )

include_directories( ${KADU_INCLUDE_DIR} )
include_directories( ${KADU_INCLUDE_DIR}/kadu-core )

set( SOURCES
	senthistory.cpp
)

set( MOC_SOURCES
	senthistory.h
)

set( TRANSLATION_SOURCES
	translations/senthistory_en.ts
	translations/senthistory_cs.ts
	translations/senthistory_pl.ts
	translations/senthistory_tr.ts
)

set( CONFIGURATION_FILES
	configuration/senthistory.ui
)

kadu_plugin( senthistory
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
