set (SOURCES
	accountdialog.cpp
	certdialog.cpp
	mail.cpp
	pop3.cpp
)

set (MOC_SOURCES
	accountdialog.h
	certdialog.h
	mail.h
	pop3.h
)

set (TRANSLATION_SOURCES
	translations/mail_pl.ts
)

set (CONFIGURATION_FILES
	configuration/mail.ui)

kadu_module_desc (mail.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (mail MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (mail ${mail} ${SOURCES} ${MOC_FILES})
add_custom_target (mail-translations DEPENDS ${TRANSLATION_FILES})
if (mail STREQUAL STATIC)
set_target_properties(mail PROPERTIES COMPILE_FLAGS "-DKADULIB" )
else (mail STREQUAL STATIC)
if (WIN32)
target_link_libraries(mail kadu_core hints)
endif (WIN32)
endif (mail STREQUAL STATIC)
add_dependencies (mail mail-translations)

install (TARGETS mail DESTINATION ${MODULEDIR})
