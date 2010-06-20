set (QT_USE_QTDBUS 1)

set (SOURCES
	kde_notify.cpp
)

set (MOC_SOURCES
	kde_notify.h
)

set (TRANSLATION_SOURCES
	translations/kde_notify_pl.ts
)

set (CONFIGURATION_FILES
	configuration/kde_notify.ui)

kadu_module_desc (kde_notify.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (kde_notify MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (kde_notify ${kde_notify} ${SOURCES} ${MOC_FILES})
add_custom_target (kde_notify-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (kde_notify kde_notify-translations)
target_link_libraries (kde_notify ${QT_QTDBUS_LIBRARY})

if (kde_notify STREQUAL STATIC)
	set_target_properties(kde_notify PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (kde_notify STREQUAL STATIC)
	install (TARGETS kde_notify DESTINATION ${MODULEDIR})
endif (kde_notify STREQUAL STATIC)

