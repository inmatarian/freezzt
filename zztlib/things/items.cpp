/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include "debug.h"
#include "zstring.h"
#include "defines.h"
#include "zztEntity.h"
#include "gameWorld.h"
#include "gameBoard.h"
#include "abstractMusicStream.h"

#include "zztThing.h"

using namespace ZZTThing;

void Bullet::exec_impl()
{
  const bool wasBlocked = blocked( mDirection );

  doMove( mDirection );
  
  if (wasBlocked) {
    doDie();
  }
}

void Bullet::interact( int old_x, int old_y, int dx, int dy )
{
  board()->handleBulletCollision( old_x+dx, old_y+dy, dx, dy, mPlayerType );
}

