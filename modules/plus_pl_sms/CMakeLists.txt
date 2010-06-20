set (SOURCES
	plus_pl_sms.cpp
	plus_pl_sms_ui_handler.cpp
	sendthread.cpp
	sms_plus_pl_gateway.cpp
)

set (MOC_SOURCES
	plus_pl_sms_ui_handler.h
	sendthread.h
	sms_plus_pl_gateway.h
)

set (TRANSLATION_SOURCES
	translations/plus_pl_sms_pl.ts
)

set (CONFIGURATION_FILES
	configuration/plus_pl_sms.ui)

set (DATA_FILES
	data/curl-ca-bundle.crt)

if(WIN32)
	include_directories (${CMAKE_CURRENT_SOURCE_DIR})
	set (SOURCES ${SOURCES} ../module.rc)
endif(WIN32)

find_package(CURL)

kadu_module_desc (plus_pl_sms.desc)
kadu_module_configuration(${CONFIGURATION_FILES})

qt_wrap_cpp (plus_pl_sms MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

install (FILES ${DATA_FILES}
	DESTINATION "share/kadu/modules/data/plus_pl_sms")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (plus_pl_sms ${plus_pl_sms} ${SOURCES} ${MOC_FILES})
add_custom_target (plus_pl_sms-translations DEPENDS ${TRANSLATION_FILES})
add_custom_target (plus_pl_sms-data DEPENDS ${DATA_FIlES})
target_link_libraries(plus_pl_sms ${CURL_LIBRARIES})
if (plus_pl_sms STREQUAL STATIC)
	set_target_properties(plus_pl_sms PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (plus_pl_sms STREQUAL STATIC)
	if (WIN32)
		target_link_libraries(plus_pl_sms kadu_core)
	endif (WIN32)
endif (plus_pl_sms STREQUAL STATIC)
add_dependencies (plus_pl_sms plus_pl_sms-translations)

install (TARGETS plus_pl_sms DESTINATION ${MODULEDIR})
