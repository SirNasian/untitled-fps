cmake_minimum_required(VERSION 4.3.3)
project(untitled-fps)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_subdirectory(vendor/enet)
add_subdirectory(vendor/glfw)


file(GLOB_RECURSE CLIENT_SOURCES
	src/client/*.c
	src/common/*.c
	vendor/glad/src/gl.c
)

file(GLOB CLIENT_INCLUDES
	vendor/enet/include
	vendor/glad/include
	vendor/stb
)

add_executable(untitled-fps)
target_sources(untitled-fps PRIVATE ${CLIENT_SOURCES})
target_include_directories(untitled-fps PRIVATE ${CLIENT_INCLUDES})
target_link_libraries(untitled-fps PRIVATE enet glfw)


file(GLOB_RECURSE SERVER_SOURCES
	src/server/*.c
	src/common/*.c
)

file(GLOB SERVER_INCLUDES
	vendor/enet/include
	vendor/stb
)

add_executable(untitled-fps-server)
target_sources(untitled-fps-server PRIVATE ${SERVER_SOURCES})
target_include_directories(untitled-fps-server PRIVATE ${SERVER_INCLUDES})
target_link_libraries(untitled-fps-server PRIVATE enet m)


if(MINGW)
	target_link_options(untitled-fps PRIVATE -static)
	target_link_options(untitled-fps-server PRIVATE -static)
endif()
