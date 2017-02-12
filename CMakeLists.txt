cmake_minimum_required (VERSION 2.6)

project(mymu)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Libuv REQUIRED)
find_package(SQLite3 REQUIRED)

add_compile_options(-std=gnu11 -Wall -fms-extensions)

include_directories(include)
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${OpenGL_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_PATH})
include_directories(${SDL_IMAGE_INCLUDE_DIRS})
include_directories(${LIBUV_INCLUDE_DIRS})
include_directories(${SDL2_IMAGE_INCLUDE_DIR})
include_directories(${SQLITE3_INCLUDE_DIRS})

set(lstd_src src/array.c src/buffer.c src/filebuf.c src/hashtable.c src/heap.c src/list.c src/vecmath.c src/log.c)
add_library(lstd ${lstd_src})


set(common_src src/itemdef.c src/item.c src/map.c src/protocol.c)
add_library(lcommon ${common_src})

set(client_src src/bmd.c src/camera.c src/light.c src/mesh.c src/renderable.c src/shader.c src/texture.c src/transformation.c src/client.c src/client_connection.c)
add_executable(client ${client_src})
target_link_libraries(client lstd lcommon m ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARIES} ${SDL2_LIBRARY} ${LIBUV_LIBRARIES} ${SDL2_IMAGE_LIBRARY})

set(server_src src/server.c src/server_connection.c)
add_executable(server ${server_src})
target_link_libraries(server lstd lcommon m ${LIBUV_LIBRARIES} ${SQLITE3_LIBRARIES})

add_custom_target(res COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res ${CMAKE_CURRENT_BINARY_DIR}/res)
