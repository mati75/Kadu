project( globalhotkeys )

cmake_minimum_required( VERSION 2.8 )
set( QT_MIN_VERSION "4.7.0" )
set( CMAKE_MIN_VERSION "2.8.0" )

set( KADU_FIND_REQUIRED true )
include( FindKadu )

include_directories( ${KADU_INCLUDE_DIR} )
include_directories( ${KADU_INCLUDE_DIR}/kadu-core )

set( SOURCES
	globalhotkeys.cpp
	api.cpp
	buddiesmenu.cpp
	conf.cpp
	functions.cpp
	globalmenu.cpp
	globalwidgetmanager.cpp
	hotkey.cpp
	hotkeyedit.cpp
	serializableqstringlist.cpp
	statusesmenu.cpp
	wideiconmenustyle.cpp
)

set( MOC_SOURCES
	globalhotkeys.h
	buddiesmenu.h
	conf.h
	functions.h
	globalmenu.h
	globalwidgetmanager.h
	hotkeyedit.h
	statusesmenu.h
)

set( TRANSLATION_SOURCES
	translations/globalhotkeys_en.ts
	translations/globalhotkeys_cs.ts
	translations/globalhotkeys_it.ts
	translations/globalhotkeys_pl.ts
)

set( CONFIGURATION_FILES
	configuration/globalhotkeys.ui
)

kadu_plugin( globalhotkeys
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
