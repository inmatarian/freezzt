cmake_minimum_required(VERSION 2.6)
project(FREEZZT)

set(SOURCES
  src/main.cpp
  src/debug.cpp
  src/page437.cpp
  src/textmodePainter.cpp
  src/gameWorld.cpp
  src/gameBoard.cpp
  src/worldLoader.cpp
  src/entityManager.cpp
  src/abstractPainter.cpp
  src/observer/abstractNotifier.cpp
)

find_package(SDL REQUIRED)

include_directories(
  ${SDL_INCLUDE_DIR}
  src
  src/observer
)

link_libraries(
  ${SDL_LIBRARY}
  SDLmain
)

set(CMAKE_CXX_FLAGS "-Wall -W -Wno-unused -fexpensive-optimizations -O2 -g")

add_executable(freezzt ${SOURCES})

