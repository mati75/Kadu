project( panelkadu )

cmake_minimum_required( VERSION 2.8 )
set( QT_MIN_VERSION "4.7.0" )
set( CMAKE_MIN_VERSION "2.8.0" )

set( KADU_FIND_REQUIRED true )
include( FindKadu )

include_directories( ${KADU_INCLUDE_DIR} )
include_directories( ${KADU_INCLUDE_DIR}/kadu-core )

set( SOURCES
	panelkadu.cpp
)

set( MOC_SOURCES
	panelkadu.h
)

set( TRANSLATION_SOURCES
	translations/panelkadu_en.ts
	translations/panelkadu_cs.ts
	translations/panelkadu_es_ES.ts
	translations/panelkadu_pl.ts
)

set( CONFIGURATION_FILES
	configuration/panelkadu.ui
)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing" )

kadu_plugin( panelkadu
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
