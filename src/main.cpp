// Insert copyright and license information here.

#include "debug.h"
#include "freezztManager.h"

using namespace std;

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

