# 3.27 required for the GLES2 component of OpenGL
cmake_minimum_required(VERSION 3.19)

project(chromono)

option(USE_OPENGL_ES "Use OpenGL ES 2.0 instead of OpenGL 3.2" OFF)
option(BUILD_FOR_WII "Build for Wii using devkitPro and OpenGX" OFF)

add_definitions(-Wall -Werror -Wno-unused-variable)

set(CMAKE_CXX_STANDARD 17)

include_directories("${chromono_SOURCE_DIR}/src")
include_directories("${chromono_SOURCE_DIR}/src/renderer")
include_directories("${chromono_SOURCE_DIR}/src/circle1d")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if (DEFINED CROSSDEPS_SYSROOT)
  message(STATUS "Configuring for sysroot in ${CROSSDEPS_SYSROOT}")

  include_directories(${CROSSDEPS_SYSROOT}/include)
  link_directories(${CROSSDEPS_SYSROOT}/lib)

  include_directories(${CROSSDEPS_SYSROOT}/include/SDL2)

  if (WIN32)
      # For static linking
      list(APPEND LIBRARIES user32 gdi32 winmm imm32 ole32 oleaut32 shell32 setupapi version uuid)
      list(PREPEND LIBRARIES mingw32 glew32s)
      add_definitions(-DGLEW_STATIC)

      list(APPEND LIBRARIES opengl32)
  elseif (APPLE)
      # macOS
      list(APPEND LIBRARIES
          iconv
          objc
          -Wl,-framework,CoreAudio
          -Wl,-framework,AudioToolbox
          -Wl,-weak_framework,CoreHaptics
          -Wl,-weak_framework,GameController
          -Wl,-framework,ForceFeedback
          -lobjc
          -Wl,-framework,CoreVideo
          -Wl,-framework,Cocoa
          -Wl,-framework,Carbon
          -Wl,-framework,IOKit
          -Wl,-weak_framework,QuartzCore
          -Wl,-weak_framework,Metal
      )

      list(APPEND LIBRARIES -Wl,-framework,OpenGL)
  else()
      # Linux
      list(APPEND LIBRARIES dl pthread m util)

      list(APPEND LIBRARIES GL)
  endif()
  list(APPEND LIBRARIES SDL2main)
  list(PREPEND LIBRARIES SDL2 vorbisfile vorbis ogg z)
elseif (BUILD_FOR_WII)
  include_directories(
      /opt/devkitpro/portlibs/ppc/include
      /opt/devkitpro/portlibs/wii/include
      /opt/devkitpro/portlibs/wii/include/SDL2
      /opt/devkitpro/libogc/include
  )

  list(APPEND LIBRARIES
    /opt/devkitpro/portlibs/wii/lib/libopengx.a
    /opt/devkitpro/portlibs/wii/lib/libSDL2.a
    /opt/devkitpro/portlibs/wii/lib/libSDL2main.a

    /opt/devkitpro/libogc/lib/wii/libwiiuse.a
    /opt/devkitpro/libogc/lib/wii/libbte.a
    /opt/devkitpro/libogc/lib/wii/libwiikeyboard.a

    /opt/devkitpro/libogc/lib/wii/libfat.a
    /opt/devkitpro/libogc/lib/wii/libaesnd.a
    /opt/devkitpro/libogc/lib/wii/libasnd.a
    /opt/devkitpro/libogc/lib/wii/libogc.a

    /opt/devkitpro/portlibs/ppc/lib/libvorbisidec.a
    /opt/devkitpro/portlibs/ppc/lib/libvorbisfile.a
    /opt/devkitpro/portlibs/ppc/lib/libogg.a

    /opt/devkitpro/portlibs/ppc/lib/libz.a
  )
else()
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
  if (USE_OPENGL_ES)
    #find_package(OpenGL REQUIRED COMPONENTS GLES2)
    pkg_check_modules(GLES2 REQUIRED glesv2)
    list(APPEND LIBRARIES ${GLES2_LIBRARIES})
    include_directories(${GLES2_INCLUDE_DIRS})
    link_directories(${GLES2_LIBRARY_DIRS})
    add_definitions(-DUSE_OPENGL_ES)
  else()
    find_package(OpenGL REQUIRED COMPONENTS OpenGL)
    list(APPEND LIBRARIES OpenGL::OpenGL)
  endif()

  include_directories(${OPENGL_INCLUDE_DIRS})
endif()

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

set(EXTRA_SOURCES "")
if (BUILD_FOR_WII)
  include_directories("${chromono_SOURCE_DIR}/src/wii")
  file(GLOB EXTRA_SOURCES "${chromono_SOURCE_DIR}/src/wii/*.cpp")
  add_definitions(-DBUILD_FOR_WII)
endif()

add_executable(chromono
    ${CORE_SOURCES} ${RENDERER_SOURCES} ${CIRCLE1D_SOURCES} ${RE_SOURCES} ${EXTRA_SOURCES})

target_link_libraries(chromono ${LIBRARIES})

install(TARGETS chromono)
install(FILES ${chromono_SOURCE_DIR}/chromono.desktop DESTINATION share/applications)
install(FILES ${chromono_SOURCE_DIR}/chromono.png DESTINATION share/pixmaps)
