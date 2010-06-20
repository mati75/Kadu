
set (SOURCES
	led_blinker.cpp
	led_notify.cpp
)

set (MOC_SOURCES
	led_blinker.h
	led_notify.h
)

set (TRANSLATION_SOURCES
	translations/led_notify_pl.ts
)

set (CONFIGURATION_FILES
	configuration/led_notify.ui
)

if(WIN32)
	set (SOURCES
		${SOURCES}
		led_driver.cpp.win32
	)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
else(WIN32)
	set (SOURCES
		${SOURCES}
		led_driver.cpp
	)
	find_package(X11 REQUIRED)
endif(WIN32)

kadu_module_desc (led_notify.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (led_notify MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (led_notify ${led_notify} ${SOURCES} ${MOC_FILES})
add_custom_target (led_notify-translations DEPENDS ${TRANSLATION_FILES})
if (led_notify STREQUAL STATIC)
	set_target_properties(led_notify PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (led_notify STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(led_notify kadu_core)
	endif (WIN32)
endif (led_notify STREQUAL STATIC)
add_dependencies (led_notify led_notify-translations)

install (TARGETS led_notify DESTINATION ${MODULEDIR})
