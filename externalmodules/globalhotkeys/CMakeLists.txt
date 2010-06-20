set (SOURCES
	globalhotkeys.cpp
	hotkey.cpp
	hotkeyedit.cpp
	wideiconsmenu.cpp
	proxystyle.cpp
)

set (MOC_SOURCES
	globalhotkeys.h
)

set (TRANSLATION_SOURCES
	translations/globalhotkeys_pl.ts
)

set (CONFIGURATION_FILES
	configuration/globalhotkeys.ui)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (globalhotkeys.desc)

kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (globalhotkeys MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (globalhotkeys ${globalhotkeys} ${SOURCES} ${MOC_FILES})
add_custom_target (globalhotkeys-translations DEPENDS ${TRANSLATION_FILES})
if (globalhotkeys STREQUAL STATIC)
	set_target_properties(globalhotkeys PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (globalhotkeys STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(globalhotkeys kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(globalhotkeys PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
endif (globalhotkeys STREQUAL STATIC)
add_dependencies (globalhotkeys globalhotkeys-translations)

target_link_libraries (globalhotkeys X11)

install (TARGETS globalhotkeys DESTINATION ${MODULEDIR})
