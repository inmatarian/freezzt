// Insert copyright and license information here.

#include "debug.h"
#include "gameBoard.h"
#include "gameBoard_p.h"
#include "gameWorld.h"
#include "abstractPainter.h"
#include "entityManager.h"
#include "zztEntity.h"

GameBoardPrivate::GameBoardPrivate( GameBoard *pSelf )
  : self( pSelf )
{
  for ( int x = 0; x < 1500; x++ ) {
    field[x] = 0;
  }
}

GameBoardPrivate::~GameBoardPrivate()
{
  for ( int x = 0; x < 1500; x++ ) {
    if ( field[x] ) {
      world->entityManager()->destroyEntity( field[x] );
    }
  }

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
    if ( d->field[x] ) {
      d->world->entityManager()->destroyEntity( d->field[x] );
    }
    d->field[x] = d->world->entityManager()->createEntity( 0, 0x07 );
  }
}

ZZTEntity *GameBoard::entity( int x, int y ) const
{
  if ( x < 0 || x >= 60 || y < 0 || y >= 25 ) {
    return 0;
  }
  return d->field[ y*25 + x ];
}

void GameBoard::setEntity( int x, int y, ZZTEntity *entity )
{
  if ( x < 0 || x >= 60 || y < 0 || y >= 25 ) {
    return;
  }

  const int index = y*25 + x;

  if ( d->field[index] ) {
    d->world->entityManager()->destroyEntity( d->field[index] );
  }
  
  d->field[index] = entity;
}

void GameBoard::paint( AbstractPainter *painter )
{
  for ( int i = 0; i<1500; i++ ) {
    ZZTEntity *entity = d->field[i];
    if (entity) {
      painter->paintChar( (i%60), (i/60), entity->tile(), entity->color() );
    }
  }
}


