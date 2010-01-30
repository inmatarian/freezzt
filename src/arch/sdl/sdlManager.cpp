/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <SDL.h>
#include "platform.h"

#include "debug.h"
#include "abstractPainter.h"
#include "textmodePainter.h"
#include "abstractEventLoop.h"
#include "sdlEventLoop.h"
#include "freezztManager.h"

#include "sdlManager.h"

class SDLManagerPrivate
{
  public:
    SDLManagerPrivate( SDLManager *pSelf );

    FreeZZTManager *pFreezztManager;

  private:
    SDLManager *self;
};

SDLManagerPrivate::SDLManagerPrivate( SDLManager *pSelf )
  : pFreezztManager(0),
    self(pSelf)
{
  /* */
}

SDLManager::SDLManager( int argc, char ** argv )
  : d( new SDLManagerPrivate(this) )
{
  d->pFreezztManager = new FreeZZTManager;
  d->pFreezztManager->parseArgs( argc, argv );
}

SDLManager::~SDLManager()
{
  delete d->pFreezztManager;
  d->pFreezztManager = 0;

  zinfo() << "Quitting SDL.";
  SDL_Quit();

  delete d;
  d = 0;
}

void SDLManager::exec()
{
  // Initialize defaults, Video and Audio subsystems
  zinfo() << "Initializing SDL.";
  int ret = SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
  if(ret==-1) { 
    zerror() << "Could not initialize SDL:" << SDL_GetError();
    return;
  }

  zinfo() << "Creating display surface.";
  SDL_Surface *display = SDL_SetVideoMode( 640, 400, 0, SDL_SWSURFACE );
  if (!display) {
    zerror() << "Could not create display:" << SDL_GetError();
    return;
  }

  SDL_EnableUNICODE(1);

  SDL_WM_SetCaption("FreeZZT", "FreeZZT");
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

  SDL_FillRect( display, 0, 0 );

  TextmodePainter sdlPainter;
  sdlPainter.setSDLSurface( display );
  d->pFreezztManager->setPainter( &sdlPainter );

  SDLEventLoop sdlEventLoop;
  sdlEventLoop.setManager(d->pFreezztManager);
  d->pFreezztManager->setEventLoop( &sdlEventLoop );

  d->pFreezztManager->exec();

  d->pFreezztManager->setPainter( 0 );
  d->pFreezztManager->setEventLoop( 0 );
}

