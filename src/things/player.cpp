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

using namespace ZZTThing;

Player::Player()  
{
  /* */
}

void Player::exec_impl()
{
  int dx = 0, dy = 0;

  if ( world()->upPressed() ) {
    dx = 0;
    dy = -1;
  }
  else if ( world()->downPressed() ) {
    dx = 0;
    dy = 1;
  }
  else if ( world()->leftPressed() ) {
    dx = -1;
    dy = 0;
  }
  else if ( world()->rightPressed() ) {
    dx = 1;
    dy = 0;
  }

  if ( dx != 0 || dy != 0 )
  {
    if ( board()->entity( xPos()+dx, yPos()+dy ).id() == ZZTEntity::Forest )
    {
      // Clear forest
      board()->clearEntity( xPos()+dx, yPos()+dy );
    }

    doMove( dx, dy );
  }
}

