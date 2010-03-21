# freezzt zztlib
cmake_minimum_required(VERSION 2.6)

set(ZZTLIB_LIBRARY_NAME zztlib)

set(ZZTLIB_SOURCES
  freezztManager.cpp
  abstractPainter.cpp
  abstractMusicStream.cpp
  debug.cpp
  gameWorld.cpp
  gameBoard.cpp
  zztEntity.cpp
  loader/worldLoader.cpp
  loader/thingFactory.cpp
  loader/zztStructs.cpp
  scroll/scrollView.cpp
  scroll/scrollModel.cpp
  things/zztThing.cpp
  things/player.cpp
  things/enemies.cpp
  things/items.cpp
)

include_directories(
  .
  loader
  painters
  scroll
  things
  zztoop
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_FLAGS "-Wall -W -Wno-unused")

add_library(${ZZTLIB_LIBRARY_NAME} ${ZZTLIB_SOURCES})

