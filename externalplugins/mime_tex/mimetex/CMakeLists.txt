set(SOURCES
	mimetex.c
	gifsave.c
)
add_definitions(-DAA)

add_executable(mimetex ${SOURCES})
if(NOT MSVC)
	target_link_libraries(mimetex m)
endif(NOT MSVC)
install(TARGETS mimetex DESTINATION ${KADU_PLUGINS_LIBDIR}/bin/mime_tex)
