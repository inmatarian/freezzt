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
  if ( world()->upPressed() ) {
    doMove( North );
  }
  else if ( world()->downPressed() ) {
    doMove( South );
  }
  else if ( world()->leftPressed() ) {
    doMove( West );
  }
  else if ( world()->rightPressed() ) {
    doMove( East );
  }
}

