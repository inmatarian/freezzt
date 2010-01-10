/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

/**
 * @mainpage Free-ZZT
 *
 * @section intro_sec Introduction
 *
 * The purpose of this project is to make a version of zzt that is as
 * compatible as possible with the zzt.exe reference implementation.
 * Because that source code has been lost, we can only attempt to do
 * this on a case-by-case basis.
 *
 * @section build_sec Building From Source
 *
 * @subsection depend_sec Dependencies
 *
 * libSDL 1.2.13 or better.
 *
 * @subsection compile_sec Compiling
 *
 * Should be this easy:
 * @li mkdir build
 * @li cd build
 * @li cmake ..
 * @li make
 *
 * @section credit_sec Credit
 *
 * @subsection Authors
 *
 * @li Inmatarian <inmatarian@gmail.com>
 *
 * @subsection license_sec Copyright and License
 *
 * Insert copyright and license information here.
 *
 */

#include <string>

#include "platform.h"

#include "debug.h"
#include "freezztManager.h"

int main( int argc, char ** argv )
{
  DebuggingStream::instance()->setLevel( DebuggingStream::INFORMATIVE );
  zinfo() << "Starting";

  FreeZZTManager freezztManager;
  freezztManager.parseArgs( argc, argv );
  freezztManager.exec();

  zinfo() << "Done.\n";
  return 0;
}

