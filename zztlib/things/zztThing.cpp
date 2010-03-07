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
#include "player.h"

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

bool AbstractThing::blocked( int dir ) const
{
  switch (dir) {
    case North: return blocked(  0, -1 ); break;
    case South: return blocked(  0,  1 ); break;
    case West:  return blocked( -1,  0 ); break;
    case East:  return blocked(  1,  0 ); break;
    default: break;
  }

  return true;
}

bool AbstractThing::blocked( int x_step, int y_step ) const
{
  return blocked( xPos(), yPos(), x_step, y_step );
}

bool AbstractThing::blocked( int old_x, int old_y, int x_step, int y_step ) const
{
  const ZZTEntity &ent = board()->entity( old_x + x_step, old_y + y_step );
  switch ( entityID() ) {
    case ZZTEntity::Star:
    case ZZTEntity::Bullet:
      return !( ent.isSwimable() || ent.isWalkable() );

    case ZZTEntity::Shark:
      return (!ent.isSwimable());

    default: break;
  }

  return (!ent.isWalkable());
}

void AbstractThing::doMove( int x_step, int y_step )
{
  doMove( xPos(), yPos(), x_step, y_step );
}

void AbstractThing::doMove( int old_x, int old_y, int x_step, int y_step )
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

  int nX = old_x + x_step;
  int nY = old_y + y_step;

  // "touch" what you're about to move into
  interact( old_x, old_y, x_step, y_step );

  // don't move off the board space
  if (nX < 0 || nX >= 60) { nX = old_x; }
  if (nY < 0 || nY >= 25) { nY = old_y; }

  // didn't move, forget it
  if (nX == old_x && nY == old_y) return;

  // push stuff out of the way
  if ( pushes() ) {
    board()->pushEntities( nX, nY, x_step, y_step );
  }

  // can't move there, somethings in the way
  if ( blocked( old_x, old_y, x_step, y_step ) ) return;

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

    case RandNotBlocked:
      trans = randNotBlockedDir();
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

int AbstractThing::translateStep( int x, int y )
{
  if ( y < 0 ) {
    if ( x != 0 ) return Idle;
    return North;
  }
  if ( y > 0 ) {
    if ( x != 0 ) return Idle;
    return South;
  }
  if ( x < 0 ) {
    if ( y != 0 ) return Idle;
    return West;
  }
  if ( x > 0 ) {
    if ( y != 0 ) return Idle;
    return East;
  }
  return Idle;
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

int AbstractThing::randNotBlockedDir()
{
  int dirs[4];
  dirs[0] = North;
  dirs[1] = South;
  dirs[2] = West;
  dirs[3] = East;

  // shuffle
  for ( int i = 0; i < 4; i++ ) {
    const int r = rand() % ( 4 - i );
    const int t = dirs[i];
    dirs[i] = dirs[r];
    dirs[r] = t;
  }

  for ( int i = 0; i < 4; i++ ) {
    if ( !blocked( dirs[i] ) )
      return dirs[i];
  }

  return Idle;
}

void AbstractThing::doShoot( int x_step, int y_step, bool playerType )
{
  board()->makeBullet( xPos() + x_step, yPos() + y_step, x_step, y_step, playerType );
}

void AbstractThing::doShoot( int dir, bool playerType )
{
  int trans = translateDir( dir );
  switch (trans) {
    case North: doShoot(  0, -1, playerType ); break;
    case South: doShoot(  0,  1, playerType ); break;
    case West:  doShoot( -1,  0, playerType ); break;
    case East:  doShoot(  1,  0, playerType ); break;
    default: break;
  }
}

void AbstractThing::doDie()
{
  board()->deleteThing( this );
}

void AbstractThing::interact( int old_x, int old_y, int dx, int dy )
{
  ZZTEntity ent = board()->entity( old_x + dx, old_y + dy );
  switch( ent.id() ) {
    case ZZTEntity::EdgeOfBoard: handleEdgeOfBoard(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::Forest:      handleForest(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::Breakable:   handleBreakable(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::Player:      handlePlayer(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::Ammo:        handleAmmo(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::Torch:       handleTorch(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::Gem:         handleGem(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::Key:         handleKey(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::Door:        handleDoor(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::Scroll:      handleScroll(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::Passage:     handlePassage(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::Bomb:        handleBomb(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::Energizer:   handleEnergizer(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::Star:        handleStar(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::Bullet:      handleBullet(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::Water:       handleWater(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::InvisibleWall:  handleIvisibleWall(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::Transporter: handleTransporter(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::HorizontalBlinkWallRay:  handleHorizontalBlinkWallRay(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::Bear:        handleBear(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::Ruffian:     handleRuffian(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::Object:      handleObject(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::Slime:       handleSlime(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::Shark:       handleShark(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::SpinningGun: handleSpinningGun(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::Lion:        handleLion(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::Tiger:       handleTiger(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::VerticalBlinkWallRay:  handleVerticalBlinkWallRay(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::CentipedeHead: handleCentipedeHead(ent, old_x, old_y, dx, dy); break;
    case ZZTEntity::CentipedeSegment: handleCentipedeSegment(ent, old_x, old_y, dx, dy); break;

    default: break;
  }
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

