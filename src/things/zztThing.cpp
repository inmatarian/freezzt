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
  
  if ( ent.id() == ZZTEntity::EmptySpace ) { return false; }

  return true;
}

void AbstractThing::doMove( int x_step, int y_step )
{
  int nX = xPos() + x_step;
  int nY = yPos() + y_step;

  // don't move off the board space
  if (nX < 0 || nX >= 60) { nX = xPos(); }
  if (nY < 0 || nY >= 25) { nY = yPos(); }

  // didn't move, forget it
  if (nX == xPos() && nY == yPos()) return;

  // can't move there, somethings in the way
  if ( blockedAt( nX, nY ) ) return;

  // get my entity
  ZZTEntity selfEnt = board()->entity( xPos(), yPos() );

  // get the neighbor's entity
  ZZTEntity newUnderEnt = board()->entity( nX, nY );

  // restore what was here before me.
  board()->setEntity( xPos(), yPos(), underEntity() );

  // push neighbor underneath
  setUnderEntity( newUnderEnt );

  // put my entity in the new spot
  board()->setEntity( nX, nY, selfEnt );

  // move to new spot
  setPos( nX, nY );
}

void AbstractThing::doMove( int direction )
{
  switch (direction) {
    case 0: doMove( 0, -1 ); break;
    case 1: doMove( 0, 1 ); break;
    case 2: doMove( -1, 0 ); break;
    case 3: 
    default: doMove( 1, 0 ); break;
  }
}

void AbstractThing::doRandomAnyMove()
{
  doMove( rand()%4 );
}

void AbstractThing::exec()
{
  if ( cycle() == 0 ||
       board()->cycle() % cycle() != 0 ) {
    return;
  }

  exec_impl();
}

