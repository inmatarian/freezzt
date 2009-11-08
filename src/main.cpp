// Insert copyright and license information here.

#include <string>
#include <iostream>

#include <SDL.h>

#include "abstractNotifier.h"
#include "abstractObserver.h"

#include "debug.h"
#include "abstractPainter.h"
#include "textmodePainter.h"
#include "gameWorld.h"
#include "worldLoader.h"
#include "gameBoard.h"

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
  SDL_Surface *display = SDL_SetVideoMode( 640, 400, 32, SDL_SWSURFACE );
  if (!display) {
    zerror() << "Could not create display:" << SDL_GetError();
  }

  SDL_FillRect( display, 0, 0 );

  GameWorld *world = 0;
  if (argc >= 2) {
    zinfo() << "Loading " << argv[1];
    world = WorldLoader::loadWorld( argv[1] );
  }
  else {
    zinfo() << "Using blank world";
    world = new GameWorld();
    GameBoard *board = new GameBoard();
    board->setWorld(world);
    world->addBoard(0, board);
  }

  world->setCurrentBoard( world->getBoard(0) );

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

    if (world) {
      world->setCurrentTimePassed( world->currentTimePassed() + 1 );
      world->paint( &painter );
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

  if (world) {
    zinfo() << "Deleting GameWorld";
    delete world;
  }

  // Shutdown all subsystems
  zinfo() << "Quitting SDL.";
  SDL_Quit();

  zinfo() << "Done.\n";
  return 0;
}

