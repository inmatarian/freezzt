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

if(UNIX)
  if(APPLE)
    set(FREEZZT_PLATFORM "MacOSX")
    set(ARCH_UTILS_DIR src/arch/default)
    set(ARCHUTILS_CPP src/arch/default/archUtils_default.cpp)
  else(APPLE)
    set(FREEZZT_PLATFORM "Linux")
    set(ARCH_UTILS_DIR src/arch/linux)
    set(ARCHUTILS_CPP src/arch/linux/archUtils_linux.cpp)
  endif(APPLE)
else(UNIX)
  if(WIN32)
    set(FREEZZT_PLATFORM "Windows")
    set(ARCH_UTILS_DIR src/arch/default)
    set(ARCHUTILS_CPP src/arch/default/archUtils_default.cpp)
  else(WIN32)
    set(FREEZZT_PLATFORM "Unknown")
    set(ARCH_UTILS_DIR src/arch/default)
    set(ARCHUTILS_CPP src/arch/default/archUtils_default.cpp)
  endif(WIN32)
endif(UNIX)
message("Detected platform is ${FREEZZT_PLATFORM}")

include_directories(
  ${SDL_INCLUDE_DIR}
  src
  src/arch
  ${ARCH_UTILS_DIR}
  zztlib
  zztlib/loader
  zztlib/painters
  zztlib/scroll
  zztlib/things
  zztlib/util
)

set(FREEZZT_SOURCES
  src/dotFileParser.cpp
  src/fileListModel.cpp
  src/main.cpp
  src/sdlEventLoop.cpp
  src/sdlManager.cpp
  src/sdlMusicStream.cpp
  src/simplePainter.cpp
  ${ARCHUTILS_CPP}
)

link_libraries(
  ${SDL_LIBRARY}
  SDLmain
  zztlib
)

set(CMAKE_CXX_FLAGS "-Wall -W -Wno-unused -DCONFIG_SDL")

add_executable(${FREEZZT_BINARY_NAME} ${FREEZZT_SOURCES})

configure_file(${CMAKE_SOURCE_DIR}/src/freezztrc
  ${CMAKE_BINARY_DIR}/freezztrc COPYONLY
)

