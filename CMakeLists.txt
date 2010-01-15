# FreeZZT Project

cmake_minimum_required(VERSION 2.6)
project(FREEZZT)

set(SOURCES
  src/main.cpp
  src/freezztManager.cpp
  src/sdlEventLoop.cpp
  src/debug.cpp
  src/dotFileParser.cpp
  src/gameWorld.cpp
  src/gameBoard.cpp
  src/zztEntity.cpp
  src/arch/platform.h
  src/loader/worldLoader.cpp
  src/loader/thingFactory.cpp
  src/loader/zztStructs.cpp
  src/painters/abstractPainter.cpp
  src/painters/textmodePainter.cpp
  src/things/zztThing.cpp
  src/things/player.cpp
  src/things/enemies.cpp
)

find_package(SDL REQUIRED)

include_directories(
  ${SDL_INCLUDE_DIR}
  src
  src/arch
  src/loader
  src/painters
  src/things
  src/zztoop
)

link_libraries(
  ${SDL_LIBRARY}
  SDLmain
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_FLAGS "-Wall -W -Wno-unused -DCONFIG_SDL")

add_executable(freezzt ${SOURCES})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/freezztrc
    ${CMAKE_CURRENT_BINARY_DIR}/freezztrc COPYONLY)
