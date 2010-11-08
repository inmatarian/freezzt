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

#include "abstractFileModelFactory.h"
#include "abstractMusicStream.h"
#include "abstractPainter.h"
#include "abstractScrollModel.h"
#include "freezztManager.h"
#include "gameBoard.h"
#include "gameWidgets.h"
#include "gameWorld.h"
#include "scrollView.h"
#include "worldLoader.h"

#include "gameControllers.h"

using namespace Defines;

ControllerInterface::ControllerInterface()
  : manager(0),
    share(0)
{
  /* */
}

void ControllerInterface::enter( FreeZZTManager *pManager, ControllerShare *pShare )
{
  assert( !manager && !share );
  manager = pManager;
  share = pShare;
  enter_impl();
}

void ControllerInterface::leave()
{
  assert( manager && share );
  leave_impl();
  manager = 0;
  share = 0;

  // would delete self if ::create instances weren't statics.
}

// ---------------------------------------------------------------------------

ControllerInterface * PlayController::create()
{
  static PlayController instance;
  return &instance;
}

void PlayController::doKeypress( int keycode, int unicode )
{
  bool filtered = false;
  switch ( keycode ) {
    case Z_Unicode:
      switch ( unicode ) {
        case 'P':
        case 'p':
          share->nextController = PauseController::create();
          filtered = true;
          break;

        case 'B':
        case 'b': {
          const bool isQuiet = share->world->musicStream()->isQuiet();
          share->world->musicStream()->setQuiet( !isQuiet );
          filtered = true;
          break;
        }

        case '?':
          share->nextController = CheatController::create();
          filtered = true;
          break;

        default: break;
      }
      break;

    case Z_Escape:
      share->nextController = TitleController::create();
      filtered = true;
      break;

    default: break;
  }

  if (!filtered) {
    share->world->addInputKey( keycode, unicode );
  }
}

void PlayController::doUpdate()
{
  share->world->update();
  if ( share->scrollView.model() ) {
    share->nextController = TextViewController::create();
  }
  else if ( share->world->isChangingBoard() ) {
    share->nextController = TransitionController::create();
    share->transitionNextBoard = share->world->changingIndex();
  }
}

void PlayController::doPaint( AbstractPainter *painter )
{
  share->world->paint( painter );
  share->playModeInfoBarWidget.doPaint( painter );
}

// ---------------------------------------------------------------------------

ControllerInterface * TitleController::create()
{
  static TitleController instance;
  return &instance;
}

void TitleController::enter_impl()
{
  GameBoard *board = share->world->getBoard( 0 );
  share->world->setCurrentBoard( board );
}

void TitleController::doKeypress( int keycode, int unicode )
{
  switch ( keycode ) {
    case Z_Unicode:
      switch ( unicode ) {
        case 'P':
        case 'p':
          share->nextController = TransitionController::create();
          share->transitionNextBoard = share->world->startBoard();
          break;
        case 'S':
        case 's':
          share->nextController = PickSpeedController::create();
          break;
        case 'W':
        case 'w':
          share->nextController = WorldMenuController::create();
          break;
        default: break;
      } break;
    case Z_Escape:
      share->quitting = true;
      break;
  }
}

void TitleController::doUpdate()
{
  share->world->update();
}

void TitleController::doPaint( AbstractPainter *painter )
{
  share->world->paint( painter );
  share->titleModeInfoBarWidget.doPaint( painter );
}

// ---------------------------------------------------------------------------

ControllerInterface * WorldMenuController::create()
{
  static WorldMenuController instance;
  return &instance;
}

void WorldMenuController::doKeypress( int keycode, int unicode )
{
  share->scrollView.doKeypress( keycode, unicode );
}

void WorldMenuController::doUpdate()
{
  share->scrollView.update();

  if ( !share->scrollView.isClosed() ) return;

  switch ( share->scrollView.action() )
  {
    case AbstractScrollModel::ChangeDirectory: {
      ZString dir = share->scrollView.data();
      share->fileModelFactory->releaseFileListModel( share->scrollView.model() );
      share->scrollView.setModel( share->fileModelFactory->acquireFileListModel(dir) );
      share->scrollView.open();
      break;
    }

    case AbstractScrollModel::LoadFile: {
      GameWorld *old = share->world;
      manager->loadWorld( share->scrollView.data().c_str() );
      if ( !share->world || share->world == old ) {
        // didn't load a world, fill it with an empty world.
        zdebug() << "Todo: Implement failed world loading.";
        manager->setWorld( GameWorld::createEmptyWorld() );
      }
      delete old;
      share->nextController = TitleController::create();
      break;
    }

    default:
      share->nextController = TitleController::create();
      break;
  }
}

void WorldMenuController::doPaint( AbstractPainter *painter )
{
  share->world->paint( painter );
  share->titleModeInfoBarWidget.doPaint( painter );
  share->scrollView.paint( painter );
}

void WorldMenuController::enter_impl()
{
  share->scrollView.setModel( share->fileModelFactory->acquireFileListModel() );
  share->scrollView.open();
}

void WorldMenuController::leave_impl()
{
  share->fileModelFactory->releaseFileListModel( share->scrollView.model() );
  share->scrollView.setModel(0);
}

// ---------------------------------------------------------------------------

ControllerInterface * TextViewController::create()
{
  static TextViewController instance;
  return &instance;
}

void TextViewController::doKeypress( int keycode, int unicode )
{
  share->scrollView.doKeypress( keycode, unicode );
}

void TextViewController::doUpdate()
{
  share->scrollView.update();

  if ( !share->scrollView.isClosed() ) return;

  switch ( share->scrollView.action() )
  {
    default:
      share->nextController = PlayController::create();
      break;
  }
}

void TextViewController::doPaint( AbstractPainter *painter )
{
  share->world->paint( painter );
  share->playModeInfoBarWidget.doPaint( painter );
  share->scrollView.paint( painter );
}

void TextViewController::enter_impl()
{
  share->scrollView.open();
}

void TextViewController::leave_impl()
{
  delete share->scrollView.model();
  share->scrollView.setModel(0);
}

// ---------------------------------------------------------------------------

ControllerInterface * CheatController::create()
{
  static CheatController instance;
  return &instance;
}

void CheatController::doKeypress( int keycode, int unicode )
{
  switch ( keycode ) {
    case Z_Enter:
      share->nextController = PlayController::create();
      break;
    default:
      share->textInputWidget.doKeypress(keycode, unicode);
      break;
  }
}

void CheatController::doPaint( AbstractPainter *painter )
{
  share->world->paint( painter );
  share->playModeInfoBarWidget.doPaint( painter );
  share->textInputWidget.doPaint( painter );
}

void CheatController::enter_impl()
{
  share->textInputWidget.reset();
}

void CheatController::leave_impl()
{
  share->world->doCheat( share->textInputWidget.value() );
}

// ---------------------------------------------------------------------------

ControllerInterface * PickSpeedController::create()
{
  static PickSpeedController instance;
  return &instance;
}

void PickSpeedController::doKeypress( int keycode, int unicode )
{
  switch ( keycode ) {
    case Z_Enter:
      share->nextController = TitleController::create();
      break;
    case Z_Unicode:
      switch ( unicode ) {
        case 'S':
        case 's':
          share->nextController = TitleController::create();
          break;
      }
      break;
    default:
      share->titleModeInfoBarWidget.framerateSliderWidget.doKeypress(keycode, unicode);
      break;
  }
}

void PickSpeedController::doPaint( AbstractPainter *painter )
{
  share->world->paint( painter );
  share->titleModeInfoBarWidget.doPaint( painter );
}

void PickSpeedController::enter_impl()
{
  share->titleModeInfoBarWidget.framerateSliderWidget.setFocused( true );
}

void PickSpeedController::leave_impl()
{
  share->world->setFrameCycle( share->titleModeInfoBarWidget.framerateSliderWidget.value() );
  share->titleModeInfoBarWidget.framerateSliderWidget.setFocused( false );
}

// ---------------------------------------------------------------------------

ControllerInterface *PauseController::create()
{
  static PauseController instance;
  return &instance;
}

void PauseController::doKeypress( int keycode, int unicode )
{
  switch ( keycode ) {
    case Z_Unicode:
      switch ( unicode ) {
        case 'P':
        case 'p':
          share->nextController = PlayController::create();
          break;
        default: break;
      }
      break;
    default: break;
  }
}

void PauseController::doPaint( AbstractPainter *painter )
{
  share->world->paint( painter );
}

void PauseController::enter_impl()
{
  // share->world->setPaused(true)
}

void PauseController::leave_impl()
{
  // share->world->setPaused(false)
}

// ---------------------------------------------------------------------------

ControllerInterface *TransitionController::create()
{
  static TransitionController instance;
  return &instance;
}

static void updateStrangeBorderTransitionClear( GameWorld *world )
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

void TransitionController::doUpdate()
{
  const int transitionSpeed = 120;
  int nextStep = clock + transitionSpeed;
  if ( nextStep >= 1500 ) nextStep = 1500;
    
  while ( clock < nextStep ) {
    const int ind = share->transitionList[clock];
    int x = ind % 60, y = ind / 60;
    share->world->setTransitionTile( x, y, forward );
    clock += 1;
  }

  if (clock == 1500) {
    clock = 0;
    if ( forward ) {
      forward = false;
      GameBoard *board = share->world->getBoard( share->transitionNextBoard );
      share->world->setCurrentBoard( board );
      std::reverse( share->transitionList.begin(), share->transitionList.end() );
      updateStrangeBorderTransitionClear( share->world );
    }
    else {
      share->nextController = PlayController::create();
    }
  }
}

void TransitionController::doPaint( AbstractPainter *painter )
{
  share->world->paint( painter );
}

void TransitionController::enter_impl()
{
  clock = 0;
  forward = true;
  std::random_shuffle( share->transitionList.begin(), share->transitionList.end() );
}

// ---------------------------------------------------------------------------

ControllerShare::ControllerShare()
 : currentController(0),
   nextController(0),
   world(0),
   fileModelFactory(0),
   musicStream(0),
   transitionNextBoard(0),
   quitting(false)
{
  transitionList.reserve(1500);
  for ( int i = 0; i < 1500; i++ ) {
    transitionList.push_back(i);
  }
}

