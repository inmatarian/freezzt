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
#include "abstractMusicStream.h"
#include "sdlMusicStream.h"
#include "freezztManager.h"

#include "abstractPlatformServices.h"
#include "sdlManager.h"

// ---------------------------------------------------------------------------
class SDLPlatformServices : public AbstractPlatformServices
{
  public: 
    TextmodePainter painter;
    SDLEventLoop eventLoop;
    SDLMusicStream musicStream;

  public: 
    virtual AbstractPainter * acquirePainter() { return &painter; };
    virtual AbstractPainter * currentPainter() { return &painter; };
    virtual void releasePainter( AbstractPainter * ) { /* */ };

    virtual AbstractEventLoop * acquireEventLoop() { return &eventLoop; };
    virtual AbstractEventLoop * currentEventLoop() { return &eventLoop; };
    virtual void releaseEventLoop( AbstractEventLoop * ) { /* */ };

    virtual AbstractMusicStream * acquireMusicStream() { return &musicStream; };
    virtual AbstractMusicStream * currentMusicStream() { return &musicStream; };
    virtual void releaseMusicStream( AbstractMusicStream * ) { /* */ };
};

// ---------------------------------------------------------------------------

class SDLManagerPrivate
{
  public:
    SDLManagerPrivate( SDLManager *pSelf );

  public:
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
  int ret = SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER);
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

  SDLPlatformServices services;
  services.painter.setSDLSurface( display );
  services.eventLoop.setManager(d->pFreezztManager);
  services.musicStream.begin();

  d->pFreezztManager->setServices( &services );
  d->pFreezztManager->exec();
  d->pFreezztManager->setServices( 0 );

  services.musicStream.end();
}

