cmake_minimum_required(VERSION 3.13)

project(chromono)

add_definitions(-Wall -Werror -Wno-error=unused-variable)

set(CMAKE_CXX_STANDARD 17)

include_directories("${chromono_SOURCE_DIR}/src")
include_directories("${chromono_SOURCE_DIR}/src/renderer")
include_directories("${chromono_SOURCE_DIR}/src/circle1d")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_package(PkgConfig REQUIRED)

pkg_check_modules(ZLIB REQUIRED zlib)
list(APPEND LIBRARIES ${ZLIB_LIBRARIES})
include_directories(${ZLIB_INCLUDE_DIRS})
link_directories(${ZLIB_LIBRARY_DIRS})

pkg_check_modules(VORBIS REQUIRED vorbisfile)
list(APPEND LIBRARIES ${VORBIS_LIBRARIES})
include_directories(${VORBIS_INCLUDE_DIRS})
link_directories(${VORBIS_LIBRARY_DIRS})

pkg_check_modules(SDL2 REQUIRED sdl2)
list(APPEND LIBRARIES ${SDL2_LIBRARIES})
include_directories(${SDL2_INCLUDE_DIRS})
link_directories(${SDL2_LIBRARY_DIRS})

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
list(APPEND LIBRARIES ${OPENGL_LIBRARIES})
include_directories(${OPENGL_INCLUDE_DIRS})

file(GLOB CORE_SOURCES "${chromono_SOURCE_DIR}/src/*.cpp")
file(GLOB RENDERER_SOURCES "${chromono_SOURCE_DIR}/src/renderer/*.cpp")
file(GLOB CIRCLE1D_SOURCES "${chromono_SOURCE_DIR}/src/circle1d/*.cpp")

set(RESOURCE_DIRS
    data/images
    data/markup
    data/shaders
    data/sounds
)

set(RESOURCES "")
foreach(RESOURCE_DIR ${RESOURCE_DIRS})
    file(GLOB RESOURCES_PART "${chromono_SOURCE_DIR}/${RESOURCE_DIR}/*")
    list(APPEND RESOURCES "${RESOURCES_PART}")
endforeach()

set(RE_SOURCES "resources.cpp")
set(RESOURCE_PACKER "${chromono_SOURCE_DIR}/packer.py")

add_custom_command(
    OUTPUT ${RE_SOURCES} resources.h
    COMMAND python3 ${RESOURCE_PACKER} ${RESOURCES} ${RE_SOURCES}
    DEPENDS ${RESOURCES} ${RESOURCE_PACKER}
)

add_executable(chromono
    ${CORE_SOURCES} ${RENDERER_SOURCES} ${CIRCLE1D_SOURCES} ${RE_SOURCES})

target_link_libraries(chromono ${LIBRARIES})

install(TARGETS chromono)
install(FILES ${chromono_SOURCE_DIR}/chromono.desktop DESTINATION share/applications)
install(FILES ${chromono_SOURCE_DIR}/chromono.png DESTINATION share/pixmaps)
