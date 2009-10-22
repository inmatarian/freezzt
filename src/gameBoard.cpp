// Insert copyright and license information here.

#include "debug.h"
#include "gameBoard.h"
#include "gameBoard_p.h"

GameBoardPrivate::GameBoardPrivate( GameBoard *pSelf )
  : self( pSelf )
{

}

GameBoardPrivate::~GameBoardPrivate()
{
  self = 0;
}

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

