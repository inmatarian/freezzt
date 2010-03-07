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
};

void Bullet::handleBreakable( const ZZTEntity &ent, int ox, int oy, int dx, int dy )
{
  // Clear breakable
  board()->clearEntity( ox+dx, oy+dy );
  musicStream()->playEvent( AbstractMusicStream::Breakable );
}

void Bullet::handlePlayer( const ZZTEntity &ent, int ox, int oy, int dx, int dy )
{
  /* */
}

void Bullet::handleObject( const ZZTEntity &ent, int ox, int oy, int dx, int dy )
{
  /* */
}

void Bullet::handleGem( const ZZTEntity &ent, int ox, int oy, int dx, int dy )
{
  // Clear gem
  board()->clearEntity( ox+dx, oy+dy );
  musicStream()->playEvent( AbstractMusicStream::Breakable );
}

void Bullet::handleBear( const ZZTEntity &ent, int ox, int oy, int dx, int dy )
{
  handleShotEnemy( ent, ox, oy, dx, dy );
}

void Bullet::handleRuffian( const ZZTEntity &ent, int ox, int oy, int dx, int dy )
{
  handleShotEnemy( ent, ox, oy, dx, dy );
}

void Bullet::handleSlime( const ZZTEntity &ent, int ox, int oy, int dx, int dy )
{
  handleShotEnemy( ent, ox, oy, dx, dy );
}

void Bullet::handleShark( const ZZTEntity &ent, int ox, int oy, int dx, int dy )
{
  handleShotEnemy( ent, ox, oy, dx, dy );
}

void Bullet::handleLion( const ZZTEntity &ent, int ox, int oy, int dx, int dy )
{
  handleShotEnemy( ent, ox, oy, dx, dy );
}

void Bullet::handleTiger( const ZZTEntity &ent, int ox, int oy, int dx, int dy )
{
  handleShotEnemy( ent, ox, oy, dx, dy );
}

void Bullet::handleCentipedeHead( const ZZTEntity &ent, int ox, int oy, int dx, int dy )
{
  handleShotEnemy( ent, ox, oy, dx, dy );
}

void Bullet::handleCentipedeSegment( const ZZTEntity &ent, int ox, int oy, int dx, int dy )
{
  handleShotEnemy( ent, ox, oy, dx, dy );
}

void Bullet::handleShotEnemy( const ZZTEntity &ent, int ox, int oy, int dx, int dy )
{
  // enemy bullets don't hurt other enemies.
  if (!mPlayerType) return;

  board()->deleteThing( ent.thing() );
  musicStream()->playEvent( AbstractMusicStream::KillEnemy );
}


