project (CppWebServer)

include(${CMAKE_FRAMEWORK_DIR}/project/project.cmake)

# --------------------- Source -----------------------

set (SOURCEFILES
	WebServer.cpp
)

set (HEADERFILES
	WebServer.h
)

# --------------------- Dependencies -----------------------

add_external(picojson
	GIT_REPOSITORY https://github.com/kazuho/picojson.git
	HEADER_ONLY
)

set(DEP_INCLUDES ${DEP_INCLUDES} ${external_dir}/picojson/src)

set(libwebsocketsArgs -DUSE_MSVC_RUNTIME_LIBRARY_DLL=ON -DLIBWEBSOCKETS_BUILD_DOCS=OFF -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DLWS_WITH_SHARED=OFF -DLWS_STATIC_PIC=ON)
add_external(WebSockets
	GIT_REPOSITORY https://github.com/warmcat/libwebsockets.git
	CMAKE_ARGS libwebsocketsArgs 
	LIB_NAME websockets
)
set(ALL_LIBS ${ALL_LIBS} crypto ssl z)

#include(${external_dir}/CPP11/CPP11.cmake)
#useCPP11()

# --------------------- Executable -----------------------

addStandardLibrary()

install(DIRECTORY ${PROJECT_SOURCE_DIR}/  DESTINATION "include" FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${PROJECT_SOURCE_DIR}/../cmake/framework/external/picojson/src/  DESTINATION "include" FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${PROJECT_SOURCE_DIR}/../cmake/framework/external/WebSockets/build/install/lib/  DESTINATION "lib" FILES_MATCHING PATTERN "*.a")
install(DIRECTORY ${PROJECT_SOURCE_DIR}/../cmake/framework/external/WebSockets/build/install/include/  DESTINATION "include" FILES_MATCHING PATTERN "*.h")
