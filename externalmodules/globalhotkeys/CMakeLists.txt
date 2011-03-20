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
	wideiconsmenu.cpp
)

set( MOC_SOURCES
	globalhotkeys.h
	buddiesmenu.h
	conf.h
	functions.h
	globalmenu.h
	globalwidgetmanager.h
	statusesmenu.h
)

set( TRANSLATION_SOURCES
	translations/globalhotkeys_en.ts
	translations/globalhotkeys_pl.ts
)

set( CONFIGURATION_FILES
	configuration/globalhotkeys.ui
)

kadu_module( globalhotkeys
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
