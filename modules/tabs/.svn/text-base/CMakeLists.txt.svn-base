set (SOURCES
	tabs.cpp
	tabwidget.cpp
)

set (MOC_SOURCES
	tabs.h
	tabwidget.h
)

set (TRANSLATION_SOURCES
	translations/tabs_pl.ts
)

set (CONFIGURATION_FILES
	configuration/tabs.ui)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (tabs.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt4_wrap_cpp (MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (tabs ${tabs} ${SOURCES} ${MOC_FILES})
add_custom_target (tabs-translations DEPENDS ${TRANSLATION_FILES})
if (tabs STREQUAL STATIC)
	set_target_properties(tabs PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (tabs STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(tabs kadu_core notify)
	endif (WIN32)
	if (APPLE)
		set_target_properties(tabs PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
endif (tabs STREQUAL STATIC)
add_dependencies (tabs tabs-translations)

install (TARGETS tabs DESTINATION ${MODULEDIR})
