set (SOURCES
	mimetex.c
	gifsave.c
)
add_definitions(-DAA)

add_executable(mimetex ${SOURCES})
target_link_libraries(mimetex m)
install (TARGETS mimetex DESTINATION lib/kadu/modules/bin/mime_tex)
