/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <list>
#include <string>

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
    void doFramerateDelay();

    void setState( GameState newState );
    void run();
    void runEventLoop();
    void runConfigState();
    void runMenuState();
    void runTransitionState();
    void runTitleState();
    void runPlayState();
    void runPauseState();
    void runGameOverState();

    void doFractal( int index, int &x, int &y );

  public:
    DotFileParser dotFile;
    GameWorld *world;
    SDL_Surface *display;
    int frames;
    int frameRate;
    int nextFrameClock;
    int lastKey;

    int transitionPrime;
    int transitionNextBoard;
    int transitionClock;

    AbstractPainter *painter;

    GameState gameState;

  private:
    FreeZZTManager *self;
};

FreeZZTManagerPrivate::FreeZZTManagerPrivate( FreeZZTManager *pSelf )
  : world(0),
    display(0),
    frames(0),
    frameRate(30),
    nextFrameClock(0),
    lastKey(0),
    transitionPrime(1),
    transitionNextBoard(0),
    painter(0),
    gameState(InitState),
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
  zdebug() << "frameRate: " << frameRate;

  // get transitionPrime
  transitionPrime = dotFile.getInt( "transition_prime", 1 );
  zdebug() << "transitionPrime: " << transitionPrime;
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

  SDL_WM_SetCaption("FreeZZT", "FreeZZT");
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

  SDL_FillRect( display, 0, 0 );

  TextmodePainter *sdlPainter = new TextmodePainter;
  sdlPainter->setSDLSurface( display );
  painter = sdlPainter;

  return true;
}

void FreeZZTManagerPrivate::doFramerateDelay()
{
  frames++;
  int clock = 0;
  do
  {
    // always delay, we're a nice process.
    SDL_Delay( 5 );
    clock = SDL_GetTicks();
  }
  while ( clock < nextFrameClock );

  // the user set framerate is for play only
  const int useFrameRate = ( gameState == TitleState ||
                             gameState == PlayState )
                           ? frameRate
                           : 50;

  const int delay = 1000 / useFrameRate;
  nextFrameClock += delay;
  if ( nextFrameClock < clock ) {
    // don't try to play catch up, forget it.
    nextFrameClock = clock + delay;
  }
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

void FreeZZTManagerPrivate::run()
{
  while ( gameState != QuitState )
  {
    switch ( gameState )
    {
      case ConfigState:     runConfigState();     break;
      case MenuState:       runMenuState();       break;
      case TransitionState: runTransitionState(); break;
      case TitleState:      runTitleState();      break;
      case PlayState:       runPlayState();       break;
      case PauseState:      runPauseState();      break;
      case GameOverState:   runGameOverState();   break;
      default: break;
    }
  }
}

void FreeZZTManagerPrivate::runEventLoop()
{
  lastKey = 0;

  SDL_Event event;
  while ( SDL_PollEvent( &event ) )
  {
    switch ( event.type )
    {
      case SDL_QUIT:
        setState( QuitState );
        break;

      case SDL_KEYDOWN:
        switch ( event.key.keysym.sym )
        {
          case SDLK_F10:
            // special case key
            setState( QuitState );
            break;

          default:
            lastKey = event.key.keysym.sym;
            break;
        }
        break;
    }
  }
}

void FreeZZTManagerPrivate::runConfigState()
{
  runEventLoop();
  setState( MenuState );
}

void FreeZZTManagerPrivate::runMenuState()
{
  runEventLoop();
  setState( TitleState );
}

void FreeZZTManagerPrivate::runTransitionState()
{
  runEventLoop();
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
    setState( PlayState );
  }

  if ( transitionClock % 75 == 0 )
  {
    painter->begin();
    world->paint( painter );
    painter->end();

    doFramerateDelay();
  }

  transitionClock += 1;
}

void FreeZZTManagerPrivate::runTitleState()
{
  runEventLoop();
  switch ( lastKey )
  {
    case SDLK_p: {
      setState( TransitionState );
      break;
    }

    default: break;
  }

  painter->begin();
  world->paint( painter );
  painter->end();

  doFramerateDelay();
}

void FreeZZTManagerPrivate::runPlayState()
{
  world->clearInputKeys();

  // playstate has it's own event loop.
  SDL_Event event;
  while ( SDL_PollEvent( &event ) )
  {
    switch ( event.type )
    {
      case SDL_QUIT:
        setState( QuitState );
        break;

      case SDL_KEYDOWN:
      {
        switch ( event.key.keysym.sym )
        {
          case SDLK_UP:
            world->startInputKey( Defines::InputUp );
            break;

          case SDLK_DOWN:
            world->startInputKey( Defines::InputDown );
            break;

          case SDLK_LEFT:
            world->startInputKey( Defines::InputLeft );
            break;

          case SDLK_RIGHT:
            world->startInputKey( Defines::InputRight );
            break;

          case SDLK_LEFTBRACKET:
          case SDLK_RIGHTBRACKET:
          {
            // level changing debug keys
            GameBoard *board = world->currentBoard();
            int index = world->indexOf(board);
            index += event.key.keysym.sym == SDLK_LEFTBRACKET ? -1 : 1;
            if ( index < 0 || index >= world->maxBoards() ) break;
            board = world->getBoard(index);
            world->setCurrentBoard( board );
            break;
          }

          case SDLK_F10:
            setState( QuitState );
            break;

          case SDLK_p: {
            setState( PauseState );
            break;
          }

          case SDLK_ESCAPE: {
            setState( TitleState );
            break;
          }
          
          default: break;            
        }
      } // keydown

      default: break;
    }
  }

  world->setCurrentTimePassed( world->currentTimePassed() + 1 );

  world->exec();

  painter->begin();
  world->paint( painter );
  painter->end();

  doFramerateDelay();
}

void FreeZZTManagerPrivate::runPauseState()
{
  runEventLoop();
  switch ( lastKey )
  {
    case SDLK_p: {
      setState( PlayState );
      break;
    }

    case SDLK_ESCAPE: {
      setState( TitleState );
      break;
    }

    default: break;
  }

  painter->begin();
  world->paint( painter );
  painter->end();

  doFramerateDelay();
}

void FreeZZTManagerPrivate::runGameOverState()
{
  runEventLoop();
  setState( TitleState );
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

  int startTime = SDL_GetTicks();

  zinfo() << "Entering event loop.";

  d->gameState = TitleState;
  d->run();

  int endTime = SDL_GetTicks();
  zinfo() << "Frames:" << d->frames 
          << " Framerate:" << (d->frames*1000)/(endTime-startTime);

  zinfo() << "Quitting SDL.";
  SDL_Quit();
}

