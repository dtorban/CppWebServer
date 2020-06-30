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

include(${external_dir}/CPP11/CPP11.cmake)
useCPP11()

find_package(Java COMPONENTS Development)
find_package(JNI)
if (${JNI_FOUND})
	message(${Java_FOUND} ${Java_JAVAC_EXECUTABLE} abc ${Java_Development_FOUND} ${JNI_FOUND})
	include_directories(
		${JNI_INCLUDE_DIRS}
	)

	file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/include)
	
	execute_process(
		COMMAND ${Java_JAVAC_EXECUTABLE} -h ${CMAKE_CURRENT_SOURCE_DIR}/src/include -d ${CMAKE_CURRENT_BINARY_DIR} --release 6 ${CMAKE_CURRENT_SOURCE_DIR}/src/CellReader.java ${CMAKE_CURRENT_SOURCE_DIR}/src/CellField.java ${CMAKE_CURRENT_SOURCE_DIR}/src/SimField.java ${CMAKE_CURRENT_SOURCE_DIR}/src/ModuleField.java ${CMAKE_CURRENT_SOURCE_DIR}/src/ClutchField.java
	)
	execute_process(
		COMMAND ${Java_JAR_EXECUTABLE} --create --file ${CMAKE_BINARY_DIR}/lib/CellReader.jar -C ${CMAKE_CURRENT_BINARY_DIR} CellReader.class -C ${CMAKE_CURRENT_BINARY_DIR} CellField.class -C ${CMAKE_CURRENT_BINARY_DIR} SimField.class -C ${CMAKE_CURRENT_BINARY_DIR} ModuleField.class -C ${CMAKE_CURRENT_BINARY_DIR} ClutchField.class
		#java -cp ../../build/lib/*.jar -Djava.library.path=../../build/lib/ CellReader
	)
endif()



# --------------------- Executable -----------------------

addStandardLibrary()

install(DIRECTORY ${PROJECT_SOURCE_DIR}/  DESTINATION "include" FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${PROJECT_SOURCE_DIR}/../cmake/framework/external/picojson/src/  DESTINATION "include" FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${PROJECT_SOURCE_DIR}/../cmake/framework/external/WebSockets/build/install/lib/  DESTINATION "lib" FILES_MATCHING PATTERN "*.a")
install(DIRECTORY ${PROJECT_SOURCE_DIR}/../cmake/framework/external/WebSockets/build/install/include/  DESTINATION "include" FILES_MATCHING PATTERN "*.h")
