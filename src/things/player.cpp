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

void Player::handleEdgeOfBoard( const ZZTEntity &ent, int dx, int dy )
{
  int dir = translateStep(dx, dy);
  int newBoard = -1;
  switch (dir) {
    case North: newBoard = board()->northExit(); break;
    case South: newBoard = board()->southExit(); break;
    case West:  newBoard = board()->westExit(); break;
    case East:  newBoard = board()->eastExit(); break;
    default: break; //wtf?
  }
  if (newBoard >= 0) {
    world()->changeActiveBoard(newBoard);
  }
}

void Player::handlePassage( const ZZTEntity &ent, int dx, int dy )
{
  Passage *passage = dynamic_cast<Passage*>( ent.thing() );
  assert( passage );

  world()->changeActiveBoard( passage->destination() );
  musicStream()->playEvent( AbstractMusicStream::Passage );
}

void Player::handleForest( const ZZTEntity &ent, int dx, int dy )
{
  board()->clearEntity( xPos()+dx, yPos()+dy );
  musicStream()->playEvent( AbstractMusicStream::Forest );
}

void Player::handleAmmo( const ZZTEntity &ent, int dx, int dy )
{
  world()->setCurrentAmmo( world()->currentAmmo() + 5 );
  board()->clearEntity( xPos()+dx, yPos()+dy );
  musicStream()->playEvent( AbstractMusicStream::Ammo );
}

void Player::handleTorch( const ZZTEntity &ent, int dx, int dy )
{
  world()->setCurrentTorches( world()->currentTorches() + 1 );
  board()->clearEntity( xPos()+dx, yPos()+dy );
  musicStream()->playEvent( AbstractMusicStream::Torch );
}

void Player::handleGem( const ZZTEntity &ent, int dx, int dy )
{
  world()->setCurrentGems( world()->currentGems() + 1 );
  world()->setCurrentHealth( world()->currentHealth() + 1 );
  world()->setCurrentScore( world()->currentScore() + 10 );
  board()->clearEntity( xPos()+dx, yPos()+dy );
  musicStream()->playEvent( AbstractMusicStream::Gem );
}

void Player::handleKey( const ZZTEntity &ent, int dx, int dy )
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
    board()->clearEntity( xPos()+dx, yPos()+dy );
    musicStream()->playEvent( AbstractMusicStream::Key );
  }
}

void Player::handleDoor( const ZZTEntity &ent, int dx, int dy )
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
    board()->clearEntity( xPos()+dx, yPos()+dy );
    musicStream()->playEvent( AbstractMusicStream::Door );
  }
}

