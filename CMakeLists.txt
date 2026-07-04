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

add_executable(untitled-fps)
target_sources(untitled-fps PRIVATE ${CLIENT_SOURCES})
target_include_directories(untitled-fps PRIVATE vendor/enet/include vendor/glad/include)
target_link_libraries(untitled-fps PRIVATE enet glfw)


file(GLOB_RECURSE SERVER_SOURCES
	src/server/*.c
	src/common/*.c
)

add_executable(untitled-fps-server)
target_sources(untitled-fps-server PRIVATE ${SERVER_SOURCES})
target_include_directories(untitled-fps-server PRIVATE vendor/enet/include)
target_link_libraries(untitled-fps-server PRIVATE enet m)
