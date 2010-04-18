/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <list>
#include <string>
#include <algorithm>
#include <cassert>

#include "freezztManager.h"

#include "debug.h"
#include "defines.h"
#include "abstractPainter.h"
#include "gameWorld.h"
#include "gameBoard.h"
#include "worldLoader.h"
#include "abstractMusicStream.h"
#include "abstractPlatformServices.h"
#include "abstractScrollModel.h"
#include "scrollView.h"
#include "gameWidgets.h"

enum GameState
{
  InitState,
  ConfigState,
  MenuState,
  TransitionState,
  TitleState,
  PickSpeedState,
  PlayState,
  PauseState,
  CheatState,
  GameOverState,
  QuitState,
  max_state
};

// ---------------------------------------------------------------------------

class FreeZZTManagerPrivate
{
  public:
    FreeZZTManagerPrivate( FreeZZTManager *pSelf );
    ~FreeZZTManagerPrivate();

    void loadSettings();
    void drawPlainWorldFrame( AbstractPainter *painter );
    void drawTitleWorldFrame( AbstractPainter *painter );
    void drawPlayWorldFrame( AbstractPainter *painter );
    void drawTitleInfoBar( AbstractPainter *painter );
    void drawPlayInfoBar( AbstractPainter *painter );
    void drawCheatInfoBar( AbstractPainter *painter );
    void drawTextInputWidget( AbstractPainter *painter );
    void drawWorldMenuInfoBar( AbstractPainter *painter );

    void setState( GameState newState );
    void runTransitionState();

    void doFractal( int index, int &x, int &y );
    void doCheat( const std::string &code );

    void runWorld();

  public:
    GameWorld *world;
    bool begun;
    int debugFrames;
    int cycleCountdown;

    int transitionPrime;
    int transitionNextBoard;
    int transitionClock;

    AbstractPlatformServices *services;

//  TODO: ADD GAME MODES
//  AbstractMode *currentMode;
//  TitleGameMode titleGameMode;
//  MenuGameMode menuGameMode;
//  TransitionGameMode transitionGameMode;
//  PlayGameMode playGameMode;
//  PauseGameMode pauseGameMode;
//  CheatGameMode cheatGameMode;

    GameState gameState;
    GameState nextState;

    TitleModeInfoBarWidget titleModeInfoBarWidget;
    PlayModeInfoBarWidget playModeInfoBarWidget;

    TextInputWidget textInputWidget;
    ScrollView worldMenuView;

  private:
    FreeZZTManager *self;
};

FreeZZTManagerPrivate::FreeZZTManagerPrivate( FreeZZTManager *pSelf )
  : world(0),
    begun(false),
    debugFrames(0),
    cycleCountdown(0),
    transitionPrime(29),
    transitionNextBoard(0),
    services(0),
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
}

void FreeZZTManagerPrivate::drawPlainWorldFrame( AbstractPainter *painter )
{
  painter->begin();
  world->paint( painter );
  painter->end();
}

void FreeZZTManagerPrivate::drawTitleWorldFrame( AbstractPainter *painter )
{
  painter->begin();
  world->paint( painter );
  drawTitleInfoBar( painter );
  painter->end();
}

void FreeZZTManagerPrivate::drawPlayWorldFrame( AbstractPainter *painter )
{
  painter->begin();
  world->paint( painter );
  drawPlayInfoBar( painter );
  painter->end();
}

void FreeZZTManagerPrivate::drawTitleInfoBar( AbstractPainter *painter )
{
  titleModeInfoBarWidget.doPaint( painter );
}

void FreeZZTManagerPrivate::drawPlayInfoBar( AbstractPainter *painter )
{
  playModeInfoBarWidget.doPaint( painter );
}

void FreeZZTManagerPrivate::drawCheatInfoBar( AbstractPainter *painter )
{
  painter->begin();
  world->paint( painter );
  drawPlayInfoBar( painter );
  drawTextInputWidget( painter );
  painter->end();
}

void FreeZZTManagerPrivate::drawWorldMenuInfoBar( AbstractPainter *painter )
{
  painter->begin();
  world->paint( painter );
  drawTitleInfoBar( painter );
  worldMenuView.paint( painter );
  painter->end();
}

void FreeZZTManagerPrivate::drawTextInputWidget( AbstractPainter *painter )
{
  textInputWidget.doPaint( painter );
}

void FreeZZTManagerPrivate::setState( GameState newState )
{
  if ( newState == gameState ) {
    return;
  }

  if (gameState == CheatState) {
    doCheat( textInputWidget.value() );
  }
  else if (gameState == MenuState) {
    services->releaseFileListModel( worldMenuView.model() );
    worldMenuView.setModel(0);
  }
  else if (gameState == PickSpeedState) {
    world->setFrameCycle( titleModeInfoBarWidget.framerateSliderWidget.value() );
    titleModeInfoBarWidget.framerateSliderWidget.setFocused( false );
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
  else if ( newState == CheatState )
  {
    textInputWidget.reset();
  }
  else if ( newState == MenuState )
  {
    worldMenuView.setModel( services->acquireFileListModel() );
    worldMenuView.open();
  }
  else if ( newState == PickSpeedState) {
    titleModeInfoBarWidget.framerateSliderWidget.setFocused( true );
  }

  gameState = newState;
}

void FreeZZTManagerPrivate::runTransitionState()
{
  const int transitionSpeed = 80;
  
  for ( int i = 0; i < transitionSpeed; i++ )
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
    transitionClock += 1;
  }
}

void FreeZZTManagerPrivate::runWorld()
{
  world->update();
}

void FreeZZTManagerPrivate::doFractal( int index, int &x, int &y )
{
  int k = index * transitionPrime % 1500;
  x = k % 60;
  y = k / 60;
}

void FreeZZTManagerPrivate::doCheat( const std::string &code )
{
  std::string c = code;
  std::transform(c.begin(), c.end(), c.begin(), tolower);
  zdebug() << "CHEAT:" << c;

  if ( c == "ammo" ) { world->setCurrentAmmo( world->currentAmmo() + 5 ); }
  else if ( c == "health" ) { world->setCurrentHealth( world->currentHealth() + 10 ); }
  else if ( c == "gems" ) { world->setCurrentGems( world->currentGems() + 5 ); }
  else if ( c == "torches" ) { world->setCurrentTorches( world->currentTorches() + 5 ); }
  else if ( c == "keys" ) {
    for ( int i = GameWorld::BLUE_DOORKEY; i <= GameWorld::WHITE_DOORKEY; i++ ) {
      world->addDoorKey(i);
    }
  }
}

/*
Clear surrounded by 4 squares     ZAP
Debug mode (OFF)                  +DEBUG
Debug mode (ON)                   -DEBUG
Illuminate room                   -DARK
Obfuscate room                    DARK
*/

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

void FreeZZTManager::loadWorld( const char *filename )
{
  d->world = WorldLoader::loadWorld( filename );
  d->world->setCurrentBoard( d->world->getBoard(0) );
  d->playModeInfoBarWidget.setWorld(d->world);
}

void FreeZZTManager::setServices( AbstractPlatformServices *services )
{
  d->services = services;
}

void FreeZZTManager::doKeypress( int keycode, int unicode )
{
  using namespace Defines;

  switch ( d->gameState )
  {
    case ConfigState:     break;

    case MenuState:
      d->worldMenuView.doKeypress( keycode, unicode );
      break;

    case TransitionState: break;

    case TitleState:
      switch ( unicode ) {
        case 'P':
        case 'p':
          d->nextState = TransitionState;
          break;
        case 'S':
        case 's':
          d->nextState = PickSpeedState;
          break;
        case 'W':
        case 'w':
          d->nextState = MenuState;
          break;
        default: break;
      }
      break; // titlestate

    case PickSpeedState:
      switch ( keycode ) {
        case Z_Enter:
          d->nextState = TitleState;
          break;
        case Z_Unicode:
          switch ( unicode ) {
            case 'S':
            case 's':
              d->nextState = TitleState;
              break;
          }
          break;
        default:
          d->titleModeInfoBarWidget.framerateSliderWidget.doKeypress(keycode, unicode);
          break;
      }
      break; // pickspeedstate

    case PlayState: {
      bool filtered = false;
      switch ( keycode ) {
        case Z_Unicode:
          switch ( unicode ) {
            case 'P':
            case 'p':
              d->nextState = PauseState;
              filtered = true;
              break;
            case '?':
              d->nextState = CheatState;
              filtered = true;
              break;
            default: break;
          }
          break;
        default: break;
      }

      if (!filtered) {
        d->world->addInputKey( keycode, unicode );
      }
      break;
    }

    case PauseState:
      switch ( keycode ) {
        case Z_Unicode:
          switch ( unicode ) {
            case 'P':
            case 'p':
              d->nextState = PlayState;
              break;
            default: break;
          }
          break;
        default: break;
      }
      break;

    case CheatState:
      switch ( keycode ) {
        case Z_Enter:
          d->nextState = PlayState;
          break;
        default:
          d->textInputWidget.doKeypress(keycode, unicode);
          break;
      }
      break;

    case GameOverState:   break;
    default: break;
  }
}

void FreeZZTManager::doUpdate()
{
  assert( d->begun );
  switch ( d->gameState )
  {
    case ConfigState:
      d->nextState = MenuState;
      break;

    case MenuState:
      if (d->worldMenuView.state() == ScrollView::Closed)
      {
        switch ( d->worldMenuView.action() )
        {
          case AbstractScrollModel::ChangeDirectory: {
            std::string dir = d->worldMenuView.data();
            d->services->releaseFileListModel( d->worldMenuView.model() );
            d->worldMenuView.setModel( d->services->acquireFileListModel(dir) );
            d->worldMenuView.open();
            break;
          }
          default:
            d->nextState = TitleState;
            break;
        }
      }
      break;

    case TransitionState:
      d->runTransitionState();
      break;

    case TitleState:
      d->runWorld();
      break;

    case PickSpeedState:
      break;

    case PlayState:
      d->runWorld();
      break;

    case CheatState:
      break;

    case PauseState:
      break;

    case GameOverState:
      d->nextState = TitleState;
      break;

    default: break;
  }

  d->setState( d->nextState );
}

void FreeZZTManager::doPaint( AbstractPainter *painter )
{
  assert( d->begun );
  switch ( d->gameState )
  {
    case MenuState:
      d->drawWorldMenuInfoBar( painter );
      break;

    case TransitionState:
      d->drawPlainWorldFrame( painter );
      break;

    case TitleState:
      d->drawTitleWorldFrame( painter );
      break;

    case PickSpeedState:
      d->drawTitleWorldFrame( painter );
      break;

    case PlayState:
      d->drawPlayWorldFrame( painter );
      break;

    case CheatState:
      d->drawCheatInfoBar( painter );
      break;

    case PauseState:
      d->drawPlainWorldFrame( painter );
      break;

    default: break;
  }
}

void FreeZZTManager::begin()
{
  assert( !d->begun );
  if (!d->world) {
    zwarn() << "No world loaded!";
    return;
  }

  AbstractMusicStream *musicStream = d->services->acquireMusicStream();
  d->world->setMusicStream( musicStream );
  d->nextState = TitleState;
  d->gameState = TitleState;
  d->begun = true;
}

void FreeZZTManager::end()
{
  assert( d->begun );
  d->begun = false;
  d->world->setMusicStream( 0 );
  AbstractMusicStream *musicStream = d->services->currentMusicStream();
  d->services->releaseMusicStream( musicStream );
}

