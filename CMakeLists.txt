cmake_minimum_required(VERSION 2.6)
project(FREEZZT)

set(SOURCES
  src/main.cpp
  src/freezztManager.cpp
  src/debug.cpp
  src/dotFileParser.cpp
  src/textmodePainter.cpp
  src/gameWorld.cpp
  src/gameBoard.cpp
  src/zztEntity.cpp
  src/abstractPainter.cpp
  src/loader/worldLoader.cpp
  src/loader/thingFactory.cpp
  src/loader/zztStructs.cpp
  src/zztoop/labelCommands.cpp
)

find_package(SDL REQUIRED)

include_directories(
  ${SDL_INCLUDE_DIR}
  src
  src/loader
  src/observer
  src/zztoop
)

link_libraries(
  ${SDL_LIBRARY}
  SDLmain
)

set(CMAKE_CXX_FLAGS "-Wall -W -Wno-unused -fexpensive-optimizations -O2 -g")

add_executable(freezzt ${SOURCES})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/freezztrc
    ${CMAKE_CURRENT_BINARY_DIR}/freezztrc COPYONLY)
