/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <list>
#include <string>

#include "debug.h"
#include "gameBoard.h"
#include "gameBoard_p.h"
#include "gameWorld.h"
#include "abstractPainter.h"
#include "zztEntity.h"
#include "zztThing.h"

GameBoardPrivate::GameBoardPrivate( GameBoard *pSelf )
  : self( pSelf )
{
  field = new ZZTEntity[1500];
}

GameBoardPrivate::~GameBoardPrivate()
{
  // clean out the thing list
  ThingList::iterator iter;
  for( iter = thingList.begin(); iter != thingList.end(); ++iter ) {
    delete *iter;
  }

  delete[] field;
  self = 0;
}

// ---------------------------------------------------------------------------

GameBoard::GameBoard()
  : d( new GameBoardPrivate(this) )
{

}

GameBoard::~GameBoard()
{
  delete d;
  d = 0;
}

void GameBoard::setWorld( GameWorld *world )
{
  d->world = world;
}

GameWorld * GameBoard::world() const
{
  return d->world;
}

void GameBoard::clear()
{
  for ( int x = 0; x < 1500; x++ ) {
    d->field[x] = ZZTEntity();
  }
}

const ZZTEntity & GameBoard::entity( int x, int y ) const
{
  if ( x < 0 || x >= 60 || y < 0 || y >= 25 ) {
    return ZZTEntity::sharedEdgeOfBoardEntity();
  }
  return d->field[ y*60 + x ];
}

void GameBoard::setEntity( int x, int y, const ZZTEntity &entity )
{
  if ( x < 0 || x >= 60 || y < 0 || y >= 25 ) {
    return;
  }

  const int index = y*60 + x;

  d->field[index] = entity;
}

void GameBoard::paint( AbstractPainter *painter )
{
  // copy thing characters out to entities
  ThingList::iterator iter;
  for( iter = d->thingList.begin(); iter != d->thingList.end(); ++iter )
  {
    ZZTThing *thing = *iter;
    const int index = thing->yPos()*60 + thing->xPos();

    ZZTEntity &entity = d->field[index];
    entity.setTile( thing->tile() );
  }

  // paint all entities
  for ( int i = 0; i<1500; i++ )
  {
    const int x = (i%60);
    const int y = (i/60);

    if ( d->world->transitionTile(x, y) ) {
      // don't paint over transition tiles
      continue;
    }

    const ZZTEntity &entity = d->field[i];
    painter->paintChar( x, y, entity.tile(), entity.color() );
  }
}

const std::string & GameBoard::message() const
{
  return d->message;
}

void GameBoard::setMessage( const std::string &mesg )
{
  d->message = mesg;
}

int GameBoard::northExit() const { return d->northExit; }
int GameBoard::southExit() const { return d->southExit; }
int GameBoard::westExit() const { return d->westExit; } 
int GameBoard::eastExit() const { return d->eastExit; }

void GameBoard::setNorthExit( int exit ) { d->northExit = exit; }
void GameBoard::setSouthExit( int exit ) { d->southExit = exit; }
void GameBoard::setWestExit( int exit ) { d->westExit = exit; }
void GameBoard::setEastExit( int exit ) { d->eastExit = exit; }

void GameBoard::addThing( ZZTThing *thing )
{
  d->thingList.push_back( thing );
}

