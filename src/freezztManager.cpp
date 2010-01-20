/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <list>
#include <string>
#include <cassert>

#include <SDL.h>

#include "abstractManager.h"
#include "freezztManager.h"

#include "debug.h"
#include "defines.h"
#include "dotFileParser.h"
#include "abstractPainter.h"
#include "textmodePainter.h"
#include "gameWorld.h"
#include "gameBoard.h"
#include "worldLoader.h"
#include "abstractEventLoop.h"
#include "sdlEventLoop.h"

enum GameState
{
  InitState,
  ConfigState,
  MenuState,
  TransitionState,
  TitleState,
  PlayState,
  PauseState,
  GameOverState,
  QuitState,
  max_state
};

class FreeZZTManagerPrivate
{
  public:
    FreeZZTManagerPrivate( FreeZZTManager *pSelf );
    ~FreeZZTManagerPrivate();

    void loadSettings();
    bool startSDL();
    void doWorldFrame();
    void doFramerateDelay();

    void setState( GameState newState );
    void runTransitionState();

    void doFractal( int index, int &x, int &y );

  public:
    DotFileParser dotFile;
    GameWorld *world;
    SDL_Surface *display;
    int frames;
    int frameRate;
    int lastFrameClock;
    int lastKey;

    int transitionPrime;
    int transitionNextBoard;
    int transitionClock;

    AbstractPainter *painter;
    AbstractEventLoop *eventLoop;

    GameState gameState;
    GameState nextState;

  private:
    FreeZZTManager *self;
};

FreeZZTManagerPrivate::FreeZZTManagerPrivate( FreeZZTManager *pSelf )
  : world(0),
    display(0),
    frames(0),
    frameRate(30),
    lastFrameClock(0),
    lastKey(0),
    transitionPrime(1),
    transitionNextBoard(0),
    painter(0),
    eventLoop(0),
    gameState(InitState),
    nextState(InitState),
    self(pSelf)
{
  /* */
}

FreeZZTManagerPrivate::~FreeZZTManagerPrivate()
{
  delete world;
  world = 0;

  delete painter;
  painter = 0;

  delete eventLoop;
  eventLoop = 0;
}

void FreeZZTManagerPrivate::loadSettings()
{
  zinfo() << "Parsing dotfile";

  // declare settings keys
  dotFile.addKey("framerate");
  dotFile.addKey("transition_prime");

  // load keys
  dotFile.load("freezztrc");

  // get frameRate
  frameRate = dotFile.getInt( "framerate", 1 );
  if ( frameRate < 1 ) frameRate = 1;
  else if ( frameRate > 60 ) frameRate = 60;
  zdebug() << "frameRate:" << frameRate;

  // get transitionPrime
  transitionPrime = dotFile.getInt( "transition_prime", 1 );
  zdebug() << "transitionPrime:" << transitionPrime;
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

  SDL_EnableUNICODE(1);

  SDL_WM_SetCaption("FreeZZT", "FreeZZT");
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

  SDL_FillRect( display, 0, 0 );

  TextmodePainter *sdlPainter = new TextmodePainter;
  sdlPainter->setSDLSurface( display );
  painter = sdlPainter;

  SDLEventLoop *sdlEventLoop = new SDLEventLoop;
  sdlEventLoop->setManager(self);
  eventLoop = sdlEventLoop;

  return true;
}

void FreeZZTManagerPrivate::doWorldFrame()
{
  painter->begin();
  world->paint( painter );
  painter->end();
  doFramerateDelay();
}

void FreeZZTManagerPrivate::doFramerateDelay()
{
  frames++;

  // the user set framerate is for play only
  const int useFrameRate = ( gameState == TitleState ||
                             gameState == PlayState )
                           ? frameRate
                           : 50;

  const int clock = eventLoop->clock();
  const int delay = 1000 / useFrameRate;
  
  int nextFrameClock = lastFrameClock + delay;

  if ( nextFrameClock < clock ) {
    // don't try to play catch up, forget it.
    nextFrameClock = clock + delay;
  }

  lastFrameClock = nextFrameClock;

  eventLoop->sleep( nextFrameClock );
}

void FreeZZTManagerPrivate::setState( GameState newState )
{
  if ( newState == gameState ) {
    return;
  }

  // ad-hoc transitions
  if ( newState == TransitionState )
  {
    if ( gameState == TitleState ) {
      transitionNextBoard = world->startBoard();
    }
    transitionClock = 0;
  }
  else if ( newState == TitleState )
  {
    GameBoard *board = world->getBoard( 0 );
    world->setCurrentBoard( board );
  }

  gameState = newState;
}

void FreeZZTManagerPrivate::runTransitionState()
{
  if ( transitionClock < 1500 ) {
    int x, y;
    doFractal( transitionClock, x, y );
    world->setTransitionTile( x, y, true );
  }
  else if ( transitionClock == 1500 ) {
    GameBoard *board = world->getBoard( transitionNextBoard );
    world->setCurrentBoard( board );
  }
  else if ( transitionClock < 3002 ) {
    const int inverse = 1500 - (transitionClock - 1500);
    int x, y;
    doFractal( inverse, x, y );
    world->setTransitionTile( x, y, false );
  }
  else if ( transitionClock == 3002 ) {
    nextState = PlayState;
  }

  if ( transitionClock % 75 == 0 ) {
    doWorldFrame();
  }

  transitionClock += 1;
}

void FreeZZTManagerPrivate::doFractal( int index, int &x, int &y )
{
  int k = index * transitionPrime % 1500;
  x = k % 60;
  y = k / 60;
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
  d->loadSettings();

  if (argc >= 2) {
    zinfo() << "Loading" << argv[1];
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

void FreeZZTManager::doKeypress( int keycode, int unicode )
{
  using namespace Defines;

  if ( keycode == Z_F10 ) {
    // during development, F10 is the auto-quit key
    d->eventLoop->stop();
    d->nextState = QuitState;
  }

  switch ( d->gameState )
  {
    case ConfigState:     break;
    case MenuState:       break;
    case TransitionState: break;

    case TitleState:
      switch ( unicode ) {
        case 'P':
        case 'p':
          d->nextState = TransitionState;
          break;
        default: break;
      }

    case PlayState:
      d->world->addInputKey( keycode, unicode );
      break;

    case PauseState:      break;
    case GameOverState:   break;
    default: break;
  }
}

/// event loop triggers frame update
void FreeZZTManager::doFrame()
{
  switch ( d->gameState )
  {
    case ConfigState:     d->nextState = MenuState; break;
    case MenuState:       d->nextState = TitleState; break;
    case TransitionState: d->runTransitionState(); break;
    case TitleState:      d->doWorldFrame(); break;

    case PlayState:
      d->world->setCurrentTimePassed( d->world->currentTimePassed() + 1 );
      d->world->exec();
      d->doWorldFrame();
      break;

    case PauseState:      d->doWorldFrame(); break;
    case GameOverState:   d->nextState = TitleState; break;
    default: break;
  }

  d->setState( d->nextState );
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

  int startTime = d->eventLoop->clock();

  d->nextState = TitleState;
  d->gameState = TitleState;

  zinfo() << "Entering event loop.";
  d->eventLoop->exec();

  int endTime = d->eventLoop->clock();
  zinfo() << "Frames:" << d->frames 
          << " Framerate:" << (d->frames*1000)/(endTime-startTime);

  zinfo() << "Quitting SDL.";
  SDL_Quit();
}

