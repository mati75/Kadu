set (SOURCES
	nextinfo.cpp
	extendedinformationwindow.cpp
)

set (MOC_SOURCES
	nextinfo.h
	extendedinformationwindow.h
)

set (TRANSLATION_SOURCES
	translations/nextinfo_pl.ts
)

set (CONFIGURATION_FILES
	configuration/nextinfo.ui)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

kadu_module_desc (nextinfo.desc)

kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (nextinfo MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (nextinfo ${nextinfo} ${SOURCES} ${MOC_FILES})
add_custom_target (nextinfo-translations DEPENDS ${TRANSLATION_FILES})
if (nextinfo STREQUAL STATIC)
set_target_properties(nextinfo PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (nextinfo STREQUAL STATIC)
if (WIN32)
target_link_libraries(nextinfo kadu_core)
endif (WIN32)
endif (nextinfo STREQUAL STATIC)
add_dependencies (nextinfo nextinfo-translations)

install (TARGETS nextinfo DESTINATION ${MODULEDIR})
