set( SOURCES
	nextinfo.cpp
	buddynextinfodata.cpp
	extendedinformationwidgets.cpp
	photowidget.cpp
)

set( MOC_SOURCES
	nextinfo.h
	extendedinformationwidgets.h
	photowidget.h
)

set( TRANSLATION_SOURCES
	translations/nextinfo_en.ts
	translations/nextinfo_pl.ts
)

set( CONFIGURATION_FILES
	configuration/nextinfo.ui
)

kadu_module( nextinfo
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
