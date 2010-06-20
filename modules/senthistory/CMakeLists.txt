set (SOURCES
	senthistory.cpp
)

set (MOC_SOURCES
	senthistory.h
)

set (TRANSLATION_SOURCES
	translations/senthistory_pl.ts
)

set (CONFIGURATION_FILES
	configuration/senthistory.ui)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (senthistory.desc)

kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (senthistory MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (senthistory ${senthistory} ${SOURCES} ${MOC_FILES})
add_custom_target (senthistory-translations DEPENDS ${TRANSLATION_FILES})
if (senthistory STREQUAL STATIC)
	set_target_properties(senthistory PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (senthistory STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(senthistory kadu_core)
	endif (WIN32)
	if (APPLE)
		set_target_properties(senthistory PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
endif (senthistory STREQUAL STATIC)
add_dependencies (senthistory senthistory-translations)

install (TARGETS senthistory DESTINATION ${MODULEDIR})
