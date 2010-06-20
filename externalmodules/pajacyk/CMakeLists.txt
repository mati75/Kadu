set (SOURCES
	pajacyk.cpp
)

set (MOC_SOURCES
	pajacyk.h
)

set (TRANSLATION_SOURCES
	translations/pajacyk_pl.ts)

set (CONFIGURATION_FILES
	configuration/pajacyk.ui)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (pajacyk.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (pajacyk MOC_FILES ${MOC_SOURCES})
qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (pajacyk ${pajacyk} ${SOURCES} ${MOC_FILES})
add_custom_target (pajacyk-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (pajacyk pajacyk-translations)
if (pajacyk STREQUAL STATIC)
	set_target_properties(pajacyk PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (pajacyk STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(pajacyk kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(pajacyk PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS pajacyk DESTINATION ${MODULEDIR})
endif (pajacyk STREQUAL STATIC)

