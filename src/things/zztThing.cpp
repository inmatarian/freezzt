/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <cstdlib>

#include "debug.h"
#include "zztEntity.h"
#include "gameWorld.h"
#include "gameBoard.h"

#include "zztThing.h"

using namespace ZZTThing;

void AbstractThing::updateEntity()
{
  // Bad place for this? consider moving it
  m_canExec = true;

  ZZTEntity ent = board()->entity( xPos(), yPos() );

  // maybe we should assert-crash the entityID part.

  if ( ent.id() == entityID() &&
       ent.tile() == tile() ) {
    return;
  }

  ent.setTile( tile() );

  board()->setEntity( xPos(), yPos(), ent );
}

bool AbstractThing::blockedAt( int x, int y ) const
{
  ZZTEntity ent = board()->entity( x, y );
  
  if ( ent.isWalkable() ) { return false; }

  return true;
}

void AbstractThing::doMove( int x_step, int y_step )
{
  // only push cardinal directions, not in diagonals or idle
  if ( ! ( ( x_step == 0 && y_step != 0 ) ||
           ( x_step != 0 && y_step == 0 ) ) ) {
    return;
  }

  // normalize to 1 or -1
  x_step = ( x_step > 0 ) ?  1
         : ( x_step < 0 ) ? -1 : 0;

  // normalize to 1 or -1
  y_step = ( y_step > 0 ) ?  1
         : ( y_step < 0 ) ? -1 : 0;

  int nX = xPos() + x_step;
  int nY = yPos() + y_step;

  // don't move off the board space
  if (nX < 0 || nX >= 60) { nX = xPos(); }
  if (nY < 0 || nY >= 25) { nY = yPos(); }

  // didn't move, forget it
  if (nX == xPos() && nY == yPos()) return;

  // push stuff out of the way
  board()->pushEntities( nX, nY, x_step, y_step );

  // can't move there, somethings in the way
  if ( blockedAt( nX, nY ) ) return;

  // pass off control to my owner
  board()->moveThing( this, nX, nY );
}

void AbstractThing::doMove( int direction )
{
  switch (direction) {
    case North: doMove(  0, -1 ); break;
    case South: doMove(  0,  1 ); break;
    case West:  doMove( -1,  0 ); break;
    case East:  doMove(  1,  0 ); break;
    default: break;
  }
}

int AbstractThing::translateDir( int dir )
{
  int trans = Idle;
  switch ( dir )
  {
    case Seek:
      trans = seekDir();
      break;

    case RandAny:
      trans = randAnyDir();
      break;

    case North:
    case South:
    case East:
    case West:
      trans = dir;
      break;
  
    case Idle:
    default:
      trans = Idle;
      break;
  }

  return trans;
}

int AbstractThing::seekDir()
{
  Player *player = board()->player();
  
  int px, py, sx, sy;
  px = player->xPos();
  py = player->yPos();
  sx = xPos();
  sy = yPos();

  int dirx, diry;
  if      ( px < sx ) dirx = West;
  else if ( px > sx ) dirx = East;
  else                dirx = Idle;

  if      ( py < sy ) diry = North;
  else if ( py > sy ) diry = South;
  else                diry = Idle;

  // aligned, return straight line direction
  if ( dirx == Idle ) return diry;
  if ( diry == Idle ) return dirx;

  // pick a random of the two possible directions
  if ( rand() % 2 == 0 ) return dirx;
  return diry;
}

int AbstractThing::randAnyDir()
{
  return (rand() % 4) + 1;
}

void AbstractThing::exec()
{
  if ( cycle() == 0 ||
       board()->cycle() % cycle() != 0 ) {
    return;
  }

  exec_impl();

  m_canExec = false;
}

