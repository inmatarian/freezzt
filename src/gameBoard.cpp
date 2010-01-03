/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <list>
#include <string>
#include <cassert>

#include "debug.h"
#include "defines.h"
#include "gameBoard.h"
#include "gameBoard_p.h"
#include "gameWorld.h"
#include "abstractPainter.h"
#include "zztEntity.h"
#include "zztThing.h"

struct ThingListSortFunctor
{
  bool operator()( ZZTThing::AbstractThing *a, ZZTThing::AbstractThing *b )
  {
    if ( a->yPos() < b->yPos() ) return true;
    return ( a->xPos() < b->xPos() );
  }
};

// ---------------------------------------------------------------------------

GameBoardPrivate::GameBoardPrivate( GameBoard *pSelf )
  : world(0),
    boardCycle(0),
    northExit(0),
    southExit(0),
    westExit(0),
    eastExit(0),
    self( pSelf )
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

void GameBoard::exec()
{
  ThingList sortedThings;
  sortedThings.assign( d->thingList.begin(), d->thingList.end() );
  ThingListSortFunctor sortie;
  sortedThings.sort( sortie );
  
  ThingList::iterator iter;
  for( iter = sortedThings.begin(); iter != sortedThings.end(); ++iter ) {
    ZZTThing::AbstractThing *thing = *iter;
    thing->exec();
  }

  // update board cycle
  d->boardCycle += 1;
}

void GameBoard::paint( AbstractPainter *painter )
{
  // copy thing characters out to entities
  ThingList::iterator iter;
  for( iter = d->thingList.begin(); iter != d->thingList.end(); ++iter )
  {
    ZZTThing::AbstractThing *thing = *iter;
    thing->updateEntity();
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

void GameBoard::addThing( ZZTThing::AbstractThing *thing )
{
  d->thingList.push_back( thing );
}

unsigned int GameBoard::cycle() const
{
  return d->boardCycle;
}

ZZTThing::Player *GameBoard::player() const
{
  assert( d->thingList.size() > 0 );

  ZZTThing::Player *player =
    dynamic_cast<ZZTThing::Player *>(d->thingList.front());

  assert( player );

  return player;
}

