// Insert copyright and license information here.

#include <string>
#include <iostream>

#include <SDL.h>

#include "observer/abstractNotifier.h"
#include "observer/abstractObserver.h"

#include "debug.h"
#include "abstractPainter.h"
#include "textmodePainter.h"
#include "gameWorld.h"
#include "worldLoader.h"

using namespace std;

int main( int argc, char ** argv )
{
  DebuggingStream::instance()->setLevel( DebuggingStream::INFORMATIVE );

  zinfo() << "Starting";

  // Initialize defaults, Video and Audio subsystems
  zinfo() << "Initializing SDL.";
  int ret = SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
  if(ret==-1) { 
    zerror() << "Could not initialize SDL:" << SDL_GetError();
    exit(-1);
  }

  zinfo() << "Creating display surface.";
  SDL_Surface *display = SDL_SetVideoMode( 640, 350, 32, SDL_SWSURFACE );
  if (!display) {
    zerror() << "Could not create display:" << SDL_GetError();
  }

  SDL_FillRect( display, 0, 0 );

  GameWorld *world = WorldLoader::loadWorld( "town.zzt" );

  TextmodePainter painter;
  painter.setSDLSurface(display);

  zinfo() << "Entering event loop.";
  SDL_Event event;

  int startTime = SDL_GetTicks();
  int frames = 0;

  bool quit = false;
  while (!quit)
  {
    painter.begin();
    for ( int i = 0; i<256; i++ ) {
      painter.paintChar( (i%32), (i/32), i, i );
    }

    if (world) {
      int x = world->currentTimePassed();
      painter.paintChar( 70, 2, '0' + (x/10000%10), 0x0f );
      painter.paintChar( 71, 2, '0' + (x/1000%10), 0x0f );
      painter.paintChar( 72, 2, '0' + (x/100%10), 0x0f );
      painter.paintChar( 73, 2, '0' + (x/10%10), 0x0f );
      painter.paintChar( 74, 2, '0' + (x%10), 0x0f );
      world->setCurrentTimePassed( world->currentTimePassed() + 1 );
    }

    painter.end();
    SDL_Flip( display );
    frames++;

    while ( SDL_PollEvent( &event ) ) {
      switch ( event.type ) {
        case SDL_QUIT:
          quit = true;
          break;
        default: break;
      }
    }
    SDL_Delay(10);
  }

  int endTime = SDL_GetTicks();
  zinfo() << "Frames:" << frames 
          << " Framerate:" << (frames*1000)/(endTime-startTime);

  // Shutdown all subsystems
  zinfo() << "Quitting SDL.";
  SDL_Quit();

  zinfo() << "Done.\n";
  return 0;
}

