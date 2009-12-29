/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

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

