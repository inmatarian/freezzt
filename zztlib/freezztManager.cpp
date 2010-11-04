/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <vector>
#include <string>
#include <algorithm>
#include <cassert>
#include <cstdlib>

#include "debug.h"
#include "zstring.h"
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

#include "freezztManager.h"

using namespace Defines;

enum GameState
{
  InitState,
  ConfigState,
  MenuState,
  TransitionOutState,
  TransitionInState,
  TitleState,
  PickSpeedState,
  PlayState,
  PauseState,
  CheatState,
  ShowTextViewState,
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

    void setState( GameState newState );

    void drawTitleWorldFrame( AbstractPainter *painter );
    void drawPlayWorldFrame( AbstractPainter *painter );
    void drawCheatInfoBar( AbstractPainter *painter );
    void drawWorldMenuInfoBar( AbstractPainter *painter );
    void drawTextWorldInfoBar( AbstractPainter *painter );

    void updateTransitionState();
    void updateStrangeBorderTransitionClear();
    void updateMenuState();
    void updateTextScrollState();

    void doCheat( const ZString &code );

  public:
    GameWorld *world;
    bool begun;
    bool quitting;
    int debugFrames;
    int cycleCountdown;

    std::vector<int> transitionList;
    int transitionNextBoard;
    int transitionClock;

    AbstractPlatformServices *services;

    GameState gameState;
    GameState nextState;

    TitleModeInfoBarWidget titleModeInfoBarWidget;
    PlayModeInfoBarWidget playModeInfoBarWidget;

    TextInputWidget textInputWidget;
    ScrollView scrollView;

  private:
    FreeZZTManager *self;
};

FreeZZTManagerPrivate::FreeZZTManagerPrivate( FreeZZTManager *pSelf )
  : world(0),
    begun(false),
    quitting(false),
    debugFrames(0),
    cycleCountdown(0),
    transitionNextBoard(0),
    services(0),
    gameState(InitState),
    nextState(InitState),
    self(pSelf)
{
  transitionList.reserve(1500);
  for ( int i = 0; i < 1500; i++ ) {
    transitionList.push_back(i);
  }
}

FreeZZTManagerPrivate::~FreeZZTManagerPrivate()
{
  delete world;
  world = 0;
}

void FreeZZTManagerPrivate::setState( GameState newState )
{
  if ( newState == gameState ) {
    return;
  }

  // exit old state
  switch ( gameState )
  {
    case CheatState:
      doCheat( textInputWidget.value() );
      break;
    case MenuState:
      services->releaseFileListModel( scrollView.model() );
      scrollView.setModel(0);
      break;
    case ShowTextViewState:
      delete scrollView.model();
      scrollView.setModel(0);
      break;
    case PickSpeedState:
      world->setFrameCycle( titleModeInfoBarWidget.framerateSliderWidget.value() );
      titleModeInfoBarWidget.framerateSliderWidget.setFocused( false );
      break;
    default: break;
  }

  // enter new state
  switch ( newState )
  {
    case TransitionOutState: {
      if ( gameState == TitleState ) {
        transitionNextBoard = world->startBoard();
      }
      else if ( gameState == PlayState ) {
        transitionNextBoard = world->changingIndex();
      }
      transitionClock = 0;
      std::random_shuffle( transitionList.begin(), transitionList.end() );
    } break;
    case TransitionInState: {
      GameBoard *board = world->getBoard( transitionNextBoard );
      world->setCurrentBoard( board );
      transitionClock = 0;
      std::reverse( transitionList.begin(), transitionList.end() );
      updateStrangeBorderTransitionClear();
    } break;
    case TitleState: {
      GameBoard *board = world->getBoard( 0 );
      world->setCurrentBoard( board );
    } break;
    case CheatState:
      textInputWidget.reset();
      break;
    case MenuState:
      scrollView.setModel( services->acquireFileListModel() );
      scrollView.open();
      break;
    case PickSpeedState:
      titleModeInfoBarWidget.framerateSliderWidget.setFocused( true );
      break;
    case ShowTextViewState:
      scrollView.open();
      break;
    default: break;
  }

  gameState = newState;
}

void FreeZZTManagerPrivate::drawTitleWorldFrame( AbstractPainter *painter )
{
  world->paint( painter );
  titleModeInfoBarWidget.doPaint( painter );
}

void FreeZZTManagerPrivate::drawPlayWorldFrame( AbstractPainter *painter )
{
  world->paint( painter );
  playModeInfoBarWidget.doPaint( painter );
}

void FreeZZTManagerPrivate::drawCheatInfoBar( AbstractPainter *painter )
{
  world->paint( painter );
  playModeInfoBarWidget.doPaint( painter );
  textInputWidget.doPaint( painter );
}

void FreeZZTManagerPrivate::drawWorldMenuInfoBar( AbstractPainter *painter )
{
  world->paint( painter );
  titleModeInfoBarWidget.doPaint( painter );
  scrollView.paint( painter );
}

void FreeZZTManagerPrivate::drawTextWorldInfoBar( AbstractPainter *painter )
{
  world->paint( painter );
  playModeInfoBarWidget.doPaint( painter );
  scrollView.paint( painter );
}

void FreeZZTManagerPrivate::updateTransitionState()
{
  const int transitionSpeed = 120;
  const bool out = ( gameState == TransitionOutState );
  
  int nextStep = transitionClock + transitionSpeed;
  if ( nextStep >= 1500 ) {
    nextState = out ? TransitionInState : PlayState;
    nextStep = 1500;
  }

  while ( transitionClock < nextStep ) {
    const int ind = transitionList[transitionClock];
    int x = ind % 60, y = ind / 60;
    world->setTransitionTile( x, y, out );
    transitionClock += 1;
  }
}

void FreeZZTManagerPrivate::updateStrangeBorderTransitionClear()
{
  for( int i = 0; i < 60; i++ ) {
    world->setTransitionTile( i,  0, false );
    world->setTransitionTile( i, 24, false );
  }
  for( int i = 1; i < 24; i++ ) {
    world->setTransitionTile(  0, i, false );
    world->setTransitionTile( 59, i, false );
  }
}

void FreeZZTManagerPrivate::updateMenuState()
{
  scrollView.update();

  if ( !scrollView.isClosed() ) return;

  switch ( scrollView.action() )
  {
    case AbstractScrollModel::ChangeDirectory: {
      ZString dir = scrollView.data();
      services->releaseFileListModel( scrollView.model() );
      scrollView.setModel( services->acquireFileListModel(dir) );
      scrollView.open();
      break;
    }

    case AbstractScrollModel::LoadFile: {
      GameWorld *old = self->world();
      self->loadWorld( scrollView.data().c_str() );
      if ( !world || world == old ) {
        // didn't load a world, fill it with an empty world.
        zdebug() << "Todo: Implement failed world loading.";
        self->setWorld( GameWorld::createEmptyWorld() );
      }
      delete old;
      nextState = TitleState;
      break;
    }

    default:
      nextState = TitleState;
      break;
  }
}

void FreeZZTManagerPrivate::updateTextScrollState()
{
  scrollView.update();

  if ( !scrollView.isClosed() ) return;

  switch ( scrollView.action() )
  {
    default:
      nextState = PlayState;
      break;
  }
}

void FreeZZTManagerPrivate::doCheat( const ZString &code )
{
  ZString c = code.upper();
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
  GameWorld *world = WorldLoader::loadWorld( filename );

  if (!world) {
    zwarn() << "World not loaded:" << filename;
    return;
  }

  setWorld( world );
}

GameWorld *FreeZZTManager::world() const
{
  return d->world;
}

void FreeZZTManager::setWorld( GameWorld *world )
{
  assert( world );

  if ( d->begun ) {
    world->setMusicStream( d->services->currentMusicStream() );
  }

  world->setFrameCycle( d->titleModeInfoBarWidget.framerateSliderWidget.value() );
  world->setCurrentBoard( world->getBoard(0) );
  world->setScrollView( &d->scrollView );
  d->playModeInfoBarWidget.setWorld(world);

  d->world = world;
}

void FreeZZTManager::setServices( AbstractPlatformServices *services )
{
  d->services = services;
}

void FreeZZTManager::setSpeed( int value )
{
  value = boundInt( 0, value, 8 );
  d->titleModeInfoBarWidget.framerateSliderWidget.setValue(value);
  if (d->world) {
    d->world->setFrameCycle( value );
  }
}

void FreeZZTManager::doKeypress( int keycode, int unicode )
{
  switch ( d->gameState )
  {
    case ConfigState:     break;

    case MenuState:
      d->scrollView.doKeypress( keycode, unicode );
      break;

    case TransitionOutState: break;
    case TransitionInState: break;

    case TitleState:
      switch ( keycode ) {
        case Z_Unicode:
          switch ( unicode ) {
            case 'P':
            case 'p':
              d->nextState = TransitionOutState;
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
          } break;
        case Z_Escape:
          d->quitting = true;
          break;
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
            case 'B':
            case 'b': {
              const bool isQuiet = d->world->musicStream()->isQuiet();
              d->world->musicStream()->setQuiet( !isQuiet );
            } break;
            case '?':
              d->nextState = CheatState;
              filtered = true;
              break;
            default: break;
          }
          break;
        case Z_Escape:
          d->nextState = TitleState;
          filtered = true;
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

    case ShowTextViewState:
      d->scrollView.doKeypress( keycode, unicode );
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
    case ConfigState:     d->nextState = MenuState;      break;
    case MenuState:       d->updateMenuState();          break;
    case TransitionOutState: d->updateTransitionState(); break;
    case TransitionInState:  d->updateTransitionState(); break;
    case TitleState:      d->world->update();            break;
    case GameOverState:   d->nextState = TitleState;     break;
    case PickSpeedState:  break;
    case CheatState:      break;
    case PauseState:      break;
    case ShowTextViewState: d->updateTextScrollState(); break;

    case PlayState: {
      d->world->update();
      if ( d->scrollView.model() ) {
        d->nextState = ShowTextViewState;
      }
      else if ( d->world->isChangingBoard() ) {
        d->nextState = TransitionOutState;
      }
      break;
    }

    default: break;
  }

  d->setState( d->nextState );
}

void FreeZZTManager::doPaint( AbstractPainter *painter )
{
  assert( d->begun );
  painter->begin();
  switch ( d->gameState )
  {
    case MenuState:          d->drawWorldMenuInfoBar( painter ); break;
    case TransitionOutState: d->world->paint( painter );         break;
    case TransitionInState:  d->world->paint( painter );         break;
    case TitleState:         d->drawTitleWorldFrame( painter );  break;
    case PickSpeedState:     d->drawTitleWorldFrame( painter );  break;
    case PlayState:          d->drawPlayWorldFrame( painter );   break;
    case CheatState:         d->drawCheatInfoBar( painter );     break;
    case PauseState:         d->world->paint( painter );         break;
    case ShowTextViewState:  d->drawTextWorldInfoBar( painter ); break;
    default: break;
  }
  painter->end();
}

void FreeZZTManager::begin()
{
  assert( !d->begun );

  if ( d->world ) {
    d->nextState = TitleState;
  }
  else {
    setWorld( GameWorld::createEmptyWorld() );
    d->nextState = MenuState;
  }

  AbstractMusicStream *musicStream = d->services->acquireMusicStream();
  d->world->setMusicStream( musicStream );
  d->begun = true;
  d->quitting = false;
  d->setState( d->nextState );
}

void FreeZZTManager::end()
{
  assert( d->begun );
  d->begun = false;
  d->world->setMusicStream( 0 );
  AbstractMusicStream *musicStream = d->services->currentMusicStream();
  d->services->releaseMusicStream( musicStream );
}

bool FreeZZTManager::quitting() const
{
  return d->quitting;
}

