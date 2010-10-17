set( SOURCES
	ledblinker.cpp
	lednotify.cpp
)

set( MOC_SOURCES
	ledblinker.h
	lednotify.h
	leddriver.h
)

if( WIN32 )
	set( SOURCES
		${SOURCES}
		leddriver.cpp.win32
	)
	include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
	set( SOURCES ${SOURCES} ../module.rc )
else( WIN32 )
	set( SOURCES
		${SOURCES}
		leddriver.cpp
	)
endif( WIN32 )


set( TRANSLATION_SOURCES
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
