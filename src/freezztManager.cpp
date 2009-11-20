// Insert copyright and license information here.

#include <list>
#include <string>

#include <SDL.h>

#include "abstractManager.h"
#include "freezztManager.h"

#include "debug.h"
#include "abstractPainter.h"
#include "textmodePainter.h"
#include "gameWorld.h"
#include "gameBoard.h"
#include "worldLoader.h"

class FreeZZTManagerPrivate
{
  public:
    FreeZZTManagerPrivate( FreeZZTManager *pSelf );
    ~FreeZZTManagerPrivate();

    bool startSDL();
    void doFramerateDelay();

  public:
    GameWorld *world;
    SDL_Surface *display;
    int frameRate;
    int nextFrameClock;

  private:
    FreeZZTManager *self;
};

FreeZZTManagerPrivate::FreeZZTManagerPrivate( FreeZZTManager *pSelf )
  : world(0),
    display(0),
    frameRate(30),
    nextFrameClock(0),
    self(pSelf)
{
  /* */
}

FreeZZTManagerPrivate::~FreeZZTManagerPrivate()
{
  if (world) {
    delete world;
    world = 0;
  }
}

bool FreeZZTManagerPrivate::startSDL()
{
  // Initialize defaults, Video and Audio subsystems
  zinfo() << "Initializing SDL.";
  int ret = SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
  if(ret==-1) { 
    zerror() << "Could not initialize SDL:" << SDL_GetError();
    return false;
  }

  zinfo() << "Creating display surface.";
  display = SDL_SetVideoMode( 640, 400, 0, SDL_SWSURFACE );
  if (!display) {
    zerror() << "Could not create display:" << SDL_GetError();
    SDL_Quit();
    return false;
  }

  SDL_FillRect( display, 0, 0 );
  return true;
}

void FreeZZTManagerPrivate::doFramerateDelay()
{
  int clock = 0;
  do
  {
    // always delay, we're a nice process.
    SDL_Delay( 5 );
    clock = SDL_GetTicks();
  }
  while ( clock < nextFrameClock );

  const int delay = 1000 / frameRate;
  nextFrameClock += delay;
  if ( nextFrameClock < clock ) {
    // don't try to play catch up, forget it.
    nextFrameClock = clock + delay;
  }
}

// ---------------------------------------------------------------------------

FreeZZTManager::FreeZZTManager()
  : d( new FreeZZTManagerPrivate(this) )
{
  /* */
}

FreeZZTManager::~FreeZZTManager()
{
  delete d;
}

void FreeZZTManager::parseArgs( int argc, char ** argv )
{
  if (argc >= 2) {
    zinfo() << "Loading " << argv[1];
    d->world = WorldLoader::loadWorld( argv[1] );
  }
  else {
    zinfo() << "Using blank world";
    d->world = new GameWorld();
    GameBoard *board = new GameBoard();
    board->setWorld(d->world);
    d->world->addBoard(0, board);
  }

  d->world->setCurrentBoard( d->world->getBoard(0) );
}

void FreeZZTManager::setFrameRate( int hertz )
{
  d->frameRate = hertz;
}

int FreeZZTManager::frameRate() const
{
  return d->frameRate;
}

void FreeZZTManager::exec()
{
  if (!d->world) {
    zwarn() << "No world loaded!";
    return;
  }

  if (!d->startSDL()) {
    return;
  }

  TextmodePainter painter;
  painter.setSDLSurface(d->display);

  int startTime = SDL_GetTicks();
  int frames = 0;

  zinfo() << "Entering event loop.";
  SDL_Event event;

  bool quit = false;
  while (!quit)
  {
    while ( SDL_PollEvent( &event ) ) {
      switch ( event.type ) {
        case SDL_QUIT:
          quit = true;
          break;
        case SDL_KEYDOWN:
          switch ( event.key.keysym.sym )
          {
            case SDLK_F10:
              quit = true;
              break;

            case SDLK_UP:
            case SDLK_DOWN:
            case SDLK_LEFT:
            case SDLK_RIGHT: {
              GameBoard *board = d->world->currentBoard();
              int index = 0;
              switch ( event.key.keysym.sym ) {
                case SDLK_UP: index = board->northExit(); break;
                case SDLK_DOWN: index = board->southExit(); break;
                case SDLK_LEFT: index = board->westExit(); break;
                case SDLK_RIGHT: index = board->eastExit(); break;
                default: break;
              }
              if ( index <= 0 || index >= d->world->maxBoards() ) break;
              board = d->world->getBoard(index);
              d->world->setCurrentBoard( board );
            } break;

            case SDLK_LEFTBRACKET:
            case SDLK_RIGHTBRACKET:
            {
              GameBoard *board = d->world->currentBoard();
              int index = d->world->indexOf(board);
              index += event.key.keysym.sym == SDLK_LEFTBRACKET ? -1 : 1;
              if ( index < 0 || index >= d->world->maxBoards() ) break;
              board = d->world->getBoard(index);
              d->world->setCurrentBoard( board );
            } break;
            
            default: break;            
          } 
          break;         
        default: break;
      }
    }

    painter.begin();

    d->world->setCurrentTimePassed( d->world->currentTimePassed() + 1 );
    d->world->paint( &painter );

    painter.end();
    SDL_Flip( d->display );
    frames++;

    d->doFramerateDelay();
  }

  int endTime = SDL_GetTicks();
  zinfo() << "Frames:" << frames 
          << " Framerate:" << (frames*1000)/(endTime-startTime);

  zinfo() << "Quitting SDL.";
  SDL_Quit();
}

