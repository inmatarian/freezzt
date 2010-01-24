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
  const bool wasBlocked = blocked( mDirection );

  doMove( mDirection );
  
  if (wasBlocked) {
    doDie();
  }
};

void Bullet::handleBreakable( const ZZTEntity &ent, int dx, int dy )
{
  // Clear breakable
  board()->clearEntity( xPos()+dx, yPos()+dy );
}

