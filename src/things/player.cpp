/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <cstdlib>

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

  if ( dx != 0 || dy != 0 )
  {
    if ( board()->entity( xPos()+dx, yPos()+dy ).id() == ZZTEntity::Forest )
    {
      // Clear forest
      board()->clearEntity( xPos()+dx, yPos()+dy );
    }

    interact( dx, dy );
    doMove( dx, dy );
  }
  else if ( sx != 0 || sy != 0 )
  {
    doShoot( sx, sy, true );
  }
}

void Player::interact( int dx, int dy )
{
  if (!blocked( dx, dy)) {
    return;
  }

  ZZTEntity ent = board()->entity( xPos()+dx, yPos()+dy );
  switch( ent.id() ) {
    case ZZTEntity::EdgeOfBoard: handleEdgeOfBoard( dx, dy ); break;
    default: break;
  }
}

void Player::handleEdgeOfBoard( int dx, int dy )
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

