/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include "debug.h"
#include "defines.h"
#include "zztEntity.h"
#include "gameWorld.h"
#include "gameBoard.h"

#include "zztThing.h"

using namespace ZZTThing;

void Bullet::exec_impl()
{
  if ( !blocked( mDirection ) ) {
    doMove( mDirection );
  }
  else {
    doDie();
  }
};

