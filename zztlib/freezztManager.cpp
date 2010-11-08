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
#include "abstractScrollModel.h"
#include "abstractFileModelFactory.h"
#include "scrollView.h"
#include "gameWidgets.h"
#include "gameControllers.h"

#include "freezztManager.h"

using namespace Defines;

class FreeZZTManagerPrivate
{
  public:
    FreeZZTManagerPrivate( FreeZZTManager *pSelf );
    ~FreeZZTManagerPrivate();

    void cycleControllers();

  public:
    ControllerShare share;
    bool begun;

  private:
    FreeZZTManager *self;
};

FreeZZTManagerPrivate::FreeZZTManagerPrivate( FreeZZTManager *pSelf )
  : begun(false),
    self(pSelf)
{
  /* */
}

FreeZZTManagerPrivate::~FreeZZTManagerPrivate()
{
  delete share.world;
}

void FreeZZTManagerPrivate::cycleControllers()
{
  if ( share.nextController == share.currentController ) return;

  if ( share.currentController ) {
    share.currentController->leave();
  }
  share.nextController->enter( self, &share );

  share.currentController = share.nextController;
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
  return d->share.world;
}

void FreeZZTManager::setWorld( GameWorld *world )
{
  assert( world );

  if ( d->begun ) {
    world->setMusicStream( d->share.musicStream );
  }

  world->setFrameCycle( d->share.titleModeInfoBarWidget.framerateSliderWidget.value() );
  world->setCurrentBoard( world->getBoard(0) );
  world->setScrollView( &d->share.scrollView );
  d->share.playModeInfoBarWidget.setWorld(world);

  d->share.world = world;
}

void FreeZZTManager::setFileModelFactory( AbstractFileModelFactory *factory )
{
  d->share.fileModelFactory = factory;
}

void FreeZZTManager::setMusicStream( AbstractMusicStream *stream )
{
  d->share.musicStream = stream;
}

void FreeZZTManager::setSpeed( int value )
{
  value = boundInt( 0, value, 8 );
  d->share.titleModeInfoBarWidget.framerateSliderWidget.setValue(value);
  if (d->share.world) {
    d->share.world->setFrameCycle( value );
  }
}

void FreeZZTManager::doKeypress( int keycode, int unicode )
{
  d->share.currentController->doKeypress( keycode, unicode );
  d->cycleControllers();
}

void FreeZZTManager::doUpdate()
{
  assert( d->begun );
  d->share.currentController->doUpdate();
  d->cycleControllers();
}

void FreeZZTManager::doPaint( AbstractPainter *painter )
{
  assert( d->begun );
  painter->begin();
  d->share.currentController->doPaint( painter );
  painter->end();
}

void FreeZZTManager::begin()
{
  assert( !d->begun );

  if ( d->share.world ) {
    d->share.nextController = TitleController::create();
  }
  else {
    setWorld( GameWorld::createEmptyWorld() );
    d->share.nextController = WorldMenuController::create();
  }

  d->share.world->setMusicStream( d->share.musicStream );
  d->begun = true;
  d->share.quitting = false;
  d->cycleControllers();
}

void FreeZZTManager::end()
{
  assert( d->begun );
  d->begun = false;
  d->share.world->setMusicStream( 0 );
}

bool FreeZZTManager::quitting() const
{
  return d->share.quitting;
}

