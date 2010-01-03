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
  if ( world()->inputKey( Defines::InputUp ) ) {
    doMove( North );
  }
  else if ( world()->inputKey( Defines::InputDown ) ) {
    doMove( South );
  }
  else if ( world()->inputKey( Defines::InputLeft ) ) {
    doMove( West );
  }
  else if ( world()->inputKey( Defines::InputRight ) ) {
    doMove( East );
  }
}

