set( SOURCES
	ledblinker.cpp
	lednotify.cpp
)

set( MOC_SOURCES
	ledblinker.h
	lednotify.h
)

if( WIN32 )
	list( APPEND SOURCES leddriver_win32.cpp ../module.rc )
	include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
else( WIN32 )
	list( APPEND SOURCES leddriver_x11.cpp )
endif( WIN32 )


set( TRANSLATION_SOURCES
	translations/lednotify_en.ts
	translations/lednotify_pl.ts
)


set( CONFIGURATION_FILES
	configuration/lednotify.ui
)


kadu_module( lednotify
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
