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
	translations/nextinfo_pl.ts
)

set( CONFIGURATION_FILES
	configuration/nextinfo.ui
)

set( QT_USE_QTSCRIPT 1 )
include( ${QT_USE_FILE} )

kadu_module( nextinfo
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)

target_link_libraries( nextinfo ${QT_QTSCRIPT_LIBRARY} )
