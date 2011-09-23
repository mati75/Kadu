set( SOURCES
	panelkadu.cpp
)

set( MOC_SOURCES
	panelkadu.h
)

set( TRANSLATION_SOURCES
	translations/panelkadu_pl.ts
)

set( CONFIGURATION_FILES
	configuration/panelkadu.ui
)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing" )

kadu_module( panelkadu
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
