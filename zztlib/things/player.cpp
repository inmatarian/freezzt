/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <cstdlib>
#include <cassert>

#include "debug.h"
#include "defines.h"
#include "zztEntity.h"
#include "gameWorld.h"
#include "gameBoard.h"
#include "abstractMusicStream.h"

#include "zztThing.h"
#include "player.h"

using namespace ZZTThing;

Player::Player()  
{
  /* */
}

void Player::exec_impl()
{
  int dx = 0, dy = 0;
  int sx = 0, sy = 0;

  if ( world()->upPressed() ) { dy = -1; }
  else if ( world()->downPressed() ) { dy = 1; }
  else if ( world()->leftPressed() ) { dx = -1; }
  else if ( world()->rightPressed() ) { dx = 1; }
  else if ( world()->shootUpPressed() ) { sy = -1; }
  else if ( world()->shootDownPressed() ) { sy = 1; }
  else if ( world()->shootLeftPressed() ) { sx = -1; }
  else if ( world()->shootRightPressed() ) { sx = 1; }

  if ( dx != 0 || dy != 0 ) {
    doMove( dx, dy );
  }
  else if ( ( sx != 0 || sy != 0) &&
            world()->currentAmmo() > 0 )
  {
    world()->setCurrentAmmo( world()->currentAmmo() - 1 );
    doShoot( sx, sy, true );
    musicStream()->playEvent( AbstractMusicStream::Shoot );
  }
}

void Player::handleEdgeOfBoard( const ZZTEntity &ent, int ox, int oy, int dx, int dy )
{
  // don't infinitely loop board switching
  if ( ox < 0 || ox >= 60 || oy < 0 || oy >= 25 ) return;

  int dir = translateStep(dx, dy);
  int newBoardIdx = -1;
  int nx = ox, ny = oy;
  switch (dir) {
    case North: newBoardIdx = board()->northExit(); ny = 24; break;
    case South: newBoardIdx = board()->southExit(); ny =  0; break;
    case West:  newBoardIdx = board()->westExit();  nx = 59; break;
    case East:  newBoardIdx = board()->eastExit();  nx =  0; break;
    default: break; //wtf?
  }
  if (newBoardIdx < 0) return;

  GameBoard *nextBoard = world()->getBoard(newBoardIdx);
  assert( nextBoard );
  Player *otherPlayer = nextBoard->player();

  const bool otherMoved = otherPlayer->tryEnterBoard( nx, ny, dx, dy );
  if (!otherMoved) return;

  world()->changeActiveBoard(newBoardIdx);
}

bool Player::tryEnterBoard( int nx, int ny, int dx, int dy )
{
  const int old_x = xPos();
  const int old_y = yPos();
 
  // allow entry if self already owns the position
  if ( nx == old_x && ny == old_y ) return true;

  // fake a normal movement.
  doMove( nx - dx, ny - dy, dx, dy );

  // what if I didn't actually move?
  if ( xPos() == old_x && yPos() == old_y ) return false;

  return true;
}

void Player::handlePassage( const ZZTEntity &ent, int ox, int oy, int dx, int dy )
{
  Passage *passage = dynamic_cast<Passage*>( ent.thing() );
  assert( passage );

  world()->changeActiveBoard( passage->destination() );
  musicStream()->playEvent( AbstractMusicStream::Passage );
}

void Player::handleForest( const ZZTEntity &ent, int ox, int oy, int dx, int dy )
{
  board()->clearEntity( ox+dx, oy+dy );
  musicStream()->playEvent( AbstractMusicStream::Forest );
}

void Player::handleAmmo( const ZZTEntity &ent, int ox, int oy, int dx, int dy )
{
  world()->setCurrentAmmo( world()->currentAmmo() + 5 );
  board()->clearEntity( ox+dx, oy+dy );
  musicStream()->playEvent( AbstractMusicStream::Ammo );
}

void Player::handleTorch( const ZZTEntity &ent, int ox, int oy, int dx, int dy )
{
  world()->setCurrentTorches( world()->currentTorches() + 1 );
  board()->clearEntity( ox+dx, oy+dy );
  musicStream()->playEvent( AbstractMusicStream::Torch );
}

void Player::handleGem( const ZZTEntity &ent, int ox, int oy, int dx, int dy )
{
  world()->setCurrentGems( world()->currentGems() + 1 );
  world()->setCurrentHealth( world()->currentHealth() + 1 );
  world()->setCurrentScore( world()->currentScore() + 10 );
  board()->clearEntity( ox+dx, oy+dy );
  musicStream()->playEvent( AbstractMusicStream::Gem );
}

void Player::handleKey( const ZZTEntity &ent, int ox, int oy, int dx, int dy )
{
  using namespace Defines;
  int c = ent.color() & 0x0f;
  int k = GameWorld::no_doorkey;
  switch ( c ) {
    case BLUE: k = GameWorld::BLUE_DOORKEY; break;
    case GREEN: k = GameWorld::GREEN_DOORKEY; break;
    case CYAN: k = GameWorld::CYAN_DOORKEY; break;
    case RED: k = GameWorld::RED_DOORKEY; break;
    case MAGENTA: k = GameWorld::PURPLE_DOORKEY; break;
    case YELLOW: k = GameWorld::YELLOW_DOORKEY; break;
    case WHITE: k = GameWorld::WHITE_DOORKEY; break;
    default: break;
  }

  if ( !world()->hasDoorKey(k) ) {
    world()->addDoorKey(k);
    board()->clearEntity( ox+dx, oy+dy );
    musicStream()->playEvent( AbstractMusicStream::Key );
  }
}

void Player::handleDoor( const ZZTEntity &ent, int ox, int oy, int dx, int dy )
{
  using namespace Defines;
  int c = ent.color() & 0xf0;
  int k = GameWorld::no_doorkey;
  switch ( c ) {
    case BG_BLUE: k = GameWorld::BLUE_DOORKEY; break;
    case BG_GREEN: k = GameWorld::GREEN_DOORKEY; break;
    case BG_CYAN: k = GameWorld::CYAN_DOORKEY; break;
    case BG_RED: k = GameWorld::RED_DOORKEY; break;
    case BG_PURPLE: k = GameWorld::PURPLE_DOORKEY; break;
    case BG_BROWN: k = GameWorld::YELLOW_DOORKEY; break;
    case BG_GRAY: k = GameWorld::WHITE_DOORKEY; break;
    default: break;
  }

  if ( world()->hasDoorKey(k) ) {
    world()->removeDoorKey(k);
    board()->clearEntity( ox+dx, oy+dy );
    musicStream()->playEvent( AbstractMusicStream::Door );
  }
}

void Player::handleScroll( const ZZTEntity &ent, int ox, int oy, int dx, int dy )
{
  ent.thing()->handleTouched();
}

void Player::handleObject( const ZZTEntity &ent, int ox, int oy, int dx, int dy )
{
  ent.thing()->handleTouched();
}

