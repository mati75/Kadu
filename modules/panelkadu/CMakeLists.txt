set (SOURCES
	panelkadu.cpp
)

set (MOC_SOURCES
	panelkadu.h
)

set (TRANSLATION_SOURCES
	translations/panelkadu_pl.ts
)

set (CONFIGURATION_FILES
	configuration/panelkadu.ui)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (panelkadu.desc)

kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (panelkadu MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (panelkadu ${panelkadu} ${SOURCES} ${MOC_FILES})
add_custom_target (panelkadu-translations DEPENDS ${TRANSLATION_FILES})
if (panelkadu STREQUAL STATIC)
	set_target_properties(panelkadu PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (panelkadu STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(panelkadu kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(panelkadu PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
endif (panelkadu STREQUAL STATIC)
add_dependencies (panelkadu panelkadu-translations)

target_link_libraries (panelkadu X11)

install (TARGETS panelkadu DESTINATION ${MODULEDIR})
