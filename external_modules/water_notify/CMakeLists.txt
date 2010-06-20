set (SOURCES
	water_notify.cpp
)

set (MOC_SOURCES
	water_notify.h
)

set (TRANSLATION_SOURCES
	translations/water_notify_pl.ts
)

set (CONFIGURATION_FILES
	configuration/water_notify.ui)

kadu_module_desc (water_notify.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (water_notify MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

pkg_search_module (DBUS REQUIRED dbus-1)

add_library (water_notify ${water_notify} ${SOURCES} ${MOC_FILES})
add_custom_target (water_notify-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (water_notify water_notify-translations)

target_link_libraries (water_notify ${DBUS_LIBRARIES})
include_directories (${DBUS_INCLUDE_DIRS})

if (water_notify STREQUAL STATIC)
	set_target_properties(water_notify PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (water_notify STREQUAL STATIC)
	install (TARGETS water_notify DESTINATION ${MODULEDIR})
endif (water_notify STREQUAL STATIC)

