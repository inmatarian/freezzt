// Insert copyright and license information here.

#include "debug.h"
#include "gameBoard.h"
#include "gameBoard_p.h"
#include "gameWorld.h"
#include "abstractPainter.h"
#include "zztEntity.h"
#include "entityManager.h"

GameBoardPrivate::GameBoardPrivate( GameBoard *pSelf )
  : self( pSelf )
{
  for ( int x = 0; x < 1500; x++ ) {
    field[x] = 0;
  }
}

GameBoardPrivate::~GameBoardPrivate()
{
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
    d->world->entityManager()->destroyEntity( d->field[x] );
    d->field[x] = d->world->entityManager()->createEntity( 0 );
  }
}

void GameBoard::paint( AbstractPainter *painter )
{
  for ( int i = 0; i<1500; i++ ) {
    ZZTEntity *entity = d->field[i];
    painter->paintChar( (i%60), (i/60), entity->tile(), entity->color() );
  }
}


