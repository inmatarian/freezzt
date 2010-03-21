#FreeZZT Project
cmake_minimum_required(VERSION 2.6)

project( FREEZZT )

set(FREEZZT_BINARY_NAME freezzt)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)

add_subdirectory(zztlib)

find_package(SDL REQUIRED)

include_directories(
  ${SDL_INCLUDE_DIR}
  src
  zztlib
  zztlib/loader
  zztlib/painters
  zztlib/scroll
  zztlib/things
  zztlib/zztoop
)

set(CMAKE_CXX_FLAGS "-Wall -W -Wno-unused -DCONFIG_SDL")

set(FREEZZT_SOURCES
  src/main.cpp
  src/dotFileParser.cpp
  src/textmodePainter.cpp
  src/fileListModel.cpp
  src/sdlManager.cpp
  src/sdlEventLoop.cpp
  src/sdlMusicStream.cpp
)

link_libraries(
  ${SDL_LIBRARY}
  SDLmain
  zztlib
)

add_executable(${FREEZZT_BINARY_NAME} ${FREEZZT_SOURCES})

configure_file(${CMAKE_SOURCE_DIR}/src/freezztrc
  ${CMAKE_BINARY_DIR}/freezztrc COPYONLY
)

