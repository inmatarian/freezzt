# freezzt-sdl
cmake_minimum_required(VERSION 2.6)

set(FREEZZT_BINARY_NAME freezzt)

set(SOURCES
  main.cpp
  platform.h
  textmodePainter.cpp
  sdlManager.cpp
  sdlEventLoop.cpp
  sdlMusicStream.cpp
)

find_package(SDL REQUIRED)

include_directories(
  ${SDL_INCLUDE_DIR}
  ../zztlib
  ../zztlib/src/loader
  ../zztlib/src/painters
  ../zztlib/src/things
  ../zztlib/src/zztoop
)

link_libraries(
  ${SDL_LIBRARY}
  SDLmain
  zztlib
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_FLAGS "-Wall -W -Wno-unused -DCONFIG_SDL")

add_executable(${FREEZZT_BINARY_NAME} ${SOURCES})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/freezztrc
  ${CMAKE_BINARY_DIR}/freezztrc COPYONLY
)

