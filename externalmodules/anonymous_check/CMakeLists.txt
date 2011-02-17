set (SOURCES
	anonymous_check.cpp
        anonymous_check-module.cpp
        anonymous_info.cpp
)

set (MOC_SOURCES
	anonymous_check.h
)

set (TRANSLATION_SOURCES
	translations/anonymous_check.ts
)

kadu_module (anonymous_check
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
)

