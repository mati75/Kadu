set (SOURCES
	mimetex.c
	gifsave.c
)
add_definitions(-DAA)

# Original mimetex has a lot of compilation warnings. We have to remove
# -Werror to let it build even though there are warnings.
STRING(REGEX REPLACE "-Werror(=.*\ )?" " " CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

add_executable(mimetex ${SOURCES})
if (NOT MSVC)
	target_link_libraries(mimetex m)
endif (NOT MSVC)
install (TARGETS mimetex DESTINATION ${KADU_INSTALL_PLUGINS_LIB_DIR}/bin/mime_tex)
