set( SOURCES
	senthistory.cpp
)

set( MOC_SOURCES
	senthistory.h
)

set( TRANSLATION_SOURCES
	translations/senthistory_en.ts
	translations/senthistory_pl.ts
)

set( CONFIGURATION_FILES
	configuration/senthistory.ui
)

kadu_module( senthistory
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
