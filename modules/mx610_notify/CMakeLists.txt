set (SOURCES
	mx610_notify.cpp)

set (MOC_SOURCES
	mx610_notify.h)

set (TRANSLATION_SOURCES
	translations/mx610_notify_pl.ts)

set (CONFIGURATION_FILES
	configuration/mx610_notify.ui)

kadu_module_desc (mx610_notify.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (mx610_notify MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (mx610_notify ${mx610_notify} ${SOURCES} ${MOC_FILES})
add_custom_target (mx610_notify-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (mx610_notify mx610_notify-translations)
if (mx610_notify STREQUAL STATIC)
	set_target_properties(mx610_notify PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (mx610_notify STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(mx610_notify kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(mx610_notify PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS mx610_notify DESTINATION ${MODULEDIR})
endif (mx610_notify STREQUAL STATIC)

