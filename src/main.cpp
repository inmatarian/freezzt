// Insert copyright and license information here.

#include <string>

#include "debug.h"
#include "freezztManager.h"
#include "dotFileParser.h"

int main( int argc, char ** argv )
{
  DebuggingStream::instance()->setLevel( DebuggingStream::INFORMATIVE );
  zinfo() << "Starting";

  DotFileParser dotFile("freezztrc");

  FreeZZTManager freezztManager;
  freezztManager.parseArgs( argc, argv );
  freezztManager.exec();

  zinfo() << "Done.\n";
  return 0;
}

