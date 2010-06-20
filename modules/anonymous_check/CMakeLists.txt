set (SOURCES
	anonymous_check.cpp
)

set (MOC_SOURCES
	anonymous_check.h
)

set (TRANSLATION_SOURCES
	translations/anonymous_check.ts
)

set (CONFIGURATION_FILES
	configuration/anonymous_check.ui
)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (anonymous_check.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (anonymous_check MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (anonymous_check ${anonymous_check} ${SOURCES} ${MOC_FILES})
add_custom_target (anonymous_check-translations DEPENDS ${TRANSLATION_FILES})
if (anonymous_check STREQUAL STATIC)
	set_target_properties(anonymous_check PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (anonymous_check STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(anonymous_check kadu_core sms)
	endif (WIN32)
endif (anonymous_check STREQUAL STATIC)
add_dependencies (anonymous_check anonymous_check-translations)

install (TARGETS anonymous_check DESTINATION ${MODULEDIR})
