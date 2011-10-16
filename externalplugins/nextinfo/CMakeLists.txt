project( nextinfo )

cmake_minimum_required( VERSION 2.8 )
set( QT_MIN_VERSION "4.7.0" )
set( CMAKE_MIN_VERSION "2.8.0" )

set( KADU_FIND_REQUIRED true )
include( FindKadu )

include_directories( ${KADU_INCLUDE_DIR} )
include_directories( ${KADU_INCLUDE_DIR}/kadu-core )

set( SOURCES
	nextinfo.cpp
	buddynextinfodata.cpp
	extendedinformationwidgets.cpp
	photowidget.cpp
)

set( MOC_SOURCES
	nextinfo.h
	buddynextinfodata.h
	extendedinformationwidgets.h
	photowidget.h
)

set( TRANSLATION_SOURCES
	translations/nextinfo_en.ts
	translations/nextinfo_cs.ts
	translations/nextinfo_pl.ts
)

set( CONFIGURATION_FILES
	configuration/nextinfo.ui
)

set( QT_USE_QTSCRIPT 1 )
include( ${QT_USE_FILE} )

kadu_plugin( nextinfo
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)

target_link_libraries( nextinfo ${QT_QTSCRIPT_LIBRARY} )
