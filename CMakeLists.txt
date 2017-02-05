cmake_minimum_required (VERSION 2.6)

project(mymu)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${project_SOURCE_DIR}/cmake")

find_package(SDL2 REQUIRED)
find_package(SDL_image REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

add_compile_options(-std=c11 -Wall)

include_directories(include)
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${OpenGL_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_PATH})
include_directories(${SDL_IMAGE_INCLUDE_DIRS})

file(GLOB src "src/*.c")

add_executable(client ${src})
target_link_libraries(client m ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARIES} SDL2 SDL2_image)
add_custom_command(TARGET client POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:client>/res)
