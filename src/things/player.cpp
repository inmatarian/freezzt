/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <cstdlib>
#include <cassert>

#include "debug.h"
#include "defines.h"
#include "zztEntity.h"
#include "gameWorld.h"
#include "gameBoard.h"

#include "zztThing.h"
#include "player.h"

using namespace ZZTThing;

Player::Player()  
{
  /* */
}

void Player::exec_impl()
{
  int dx = 0, dy = 0;
  int sx = 0, sy = 0;

  if ( world()->upPressed() ) { dy = -1; }
  else if ( world()->downPressed() ) { dy = 1; }
  else if ( world()->leftPressed() ) { dx = -1; }
  else if ( world()->rightPressed() ) { dx = 1; }
  else if ( world()->shootUpPressed() ) { sy = -1; }
  else if ( world()->shootDownPressed() ) { sy = 1; }
  else if ( world()->shootLeftPressed() ) { sx = -1; }
  else if ( world()->shootRightPressed() ) { sx = 1; }

  if ( dx != 0 || dy != 0 ) {
    doMove( dx, dy );
  }
  else if ( sx != 0 || sy != 0 ) {
    doShoot( sx, sy, true );
  }
}

void Player::handleEdgeOfBoard( const ZZTEntity &ent, int dx, int dy )
{
  int dir = translateStep(dx, dy);
  int newBoard = -1;
  switch (dir) {
    case North: newBoard = board()->northExit(); break;
    case South: newBoard = board()->southExit(); break;
    case West:  newBoard = board()->westExit(); break;
    case East:  newBoard = board()->eastExit(); break;
    default: break; //wtf?
  }
  if (newBoard >= 0) {
    world()->changeActiveBoard(newBoard);
  }
}

void Player::handlePassage( const ZZTEntity &ent, int dx, int dy )
{
  Passage *passage = dynamic_cast<Passage*>( ent.thing() );
  assert( passage );

  world()->changeActiveBoard( passage->destination() );
}

void Player::handleForest( const ZZTEntity &ent, int dx, int dy )
{
  board()->clearEntity( xPos()+dx, yPos()+dy );
}

void Player::handleAmmo( const ZZTEntity &ent, int dx, int dy )
{
  board()->clearEntity( xPos()+dx, yPos()+dy );
}

void Player::handleTorch( const ZZTEntity &ent, int dx, int dy )
{
  board()->clearEntity( xPos()+dx, yPos()+dy );
}

void Player::handleGem( const ZZTEntity &ent, int dx, int dy )
{
  board()->clearEntity( xPos()+dx, yPos()+dy );
}

