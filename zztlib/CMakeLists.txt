# freezzt zztlib
cmake_minimum_required(VERSION 2.6)

set(ZZTLIB_LIBRARY_NAME zztlib)

set(ZZTLIB_SOURCES
  abstractMusicStream.cpp
  abstractPainter.cpp
  debug.cpp
  freezztManager.cpp
  gameBoard.cpp
  gameWidgets.cpp
  gameWorld.cpp
  zztEntity.cpp
  loader/thingFactory.cpp
  loader/worldLoader.cpp
  loader/zztStructs.cpp
  scroll/scrollView.cpp
  scroll/textScrollModel.cpp
  things/enemies.cpp
  things/items.cpp
  things/player.cpp
  things/zztThing.cpp
)

include_directories(
  .
  loader
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

