cmake_minimum_required(VERSION 4.3.3)
project(untitled-fps)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_subdirectory(vendor/enet)
add_subdirectory(vendor/glfw)

add_executable(untitled-fps)
target_sources(untitled-fps PRIVATE src/client.c vendor/glad/src/gl.c)
target_include_directories(untitled-fps PRIVATE vendor/enet/include vendor/glad/include)
target_link_libraries(untitled-fps PRIVATE enet glfw)

add_executable(untitled-fps-server)
target_sources(untitled-fps-server PRIVATE src/server.c)
target_include_directories(untitled-fps-server PRIVATE vendor/enet/include)
target_link_libraries(untitled-fps-server PRIVATE enet)
