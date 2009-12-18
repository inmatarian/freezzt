// Insert copyright and license information here.

#include <string>
#include <map>
#include <list>
#include <sstream>

#include "debug.h"
#include "gameWorld.h"
#include "gameWorld_p.h"
#include "gameBoard.h"
#include "abstractPainter.h"

GameWorldPrivate::GameWorldPrivate( GameWorld *pSelf )
  : currentAmmo( 0 ),
    currentGems( 0 ),
    currentHealth( 0 ),
    currentTorches( 0 ),
    currentTorchCycles( 0 ),
    currentScore( 0 ),
    currentEnergizerCycles( 0 ),
    currentTimePassed( 0 ),
    maxBoards( 0 ),
    currentBoard( 0 ),
    self(pSelf)
{
  for ( int x = GameWorld::BLUE_DOORKEY; x < GameWorld::max_doorkey; x++ ) {
    doorKeys[x] = false;
  }
}

GameWorldPrivate::~GameWorldPrivate()
{
  // boards need to clean up their entities, they go first
  GameBoardMap::iterator iter;
  for( iter = boards.begin(); iter != boards.end(); ++iter ) {
    delete (*iter).second;
  }

  self = 0;
}

// ---------------------------------------------------------------------------

GameWorld::GameWorld()
  : d( new GameWorldPrivate(this) )
{
  /* */
}

GameWorld::~GameWorld()
{
  delete d;
  d = 0;
}

void GameWorld::setCurrentAmmo( int ammo )
{
  d->currentAmmo = ammo;
}

int GameWorld::currentAmmo() const
{
  return d->currentAmmo;
}

void GameWorld::setCurrentGems( int gems )
{
  d->currentGems = gems;
}

int GameWorld::currentGems() const
{
  return d->currentGems;
}

void GameWorld::setCurrentHealth( int hp )
{
  d->currentHealth = hp;
}

int GameWorld::currentHealth() const
{
  return d->currentHealth;
}

void GameWorld::setCurrentTorches( int torch )
{
  d->currentTorches = torch;
}

int GameWorld::currentTorches() const
{
  return d->currentTorches;
}

void GameWorld::setCurrentTorchCycles( int cycles )
{
  d->currentTorchCycles = cycles;
}

int GameWorld::currentTorchCycles() const
{
  return d->currentTorchCycles;
}

void GameWorld::setCurrentScore( int score )
{
  d->currentScore = score;
}

int GameWorld::currentScore() const
{
  return d->currentScore;
}

void GameWorld::setCurrentEnergizerCycles( int cycles )
{
  d->currentEnergizerCycles = cycles;
}

int GameWorld::currentEnergizerCycles() const
{
  return d->currentEnergizerCycles;
}

void GameWorld::setCurrentTimePassed( int time )
{
  d->currentTimePassed = time;
}

int GameWorld::currentTimePassed() const
{
  return d->currentTimePassed;
}

void GameWorld::addDoorKey( int keyType )
{
  if ( keyType <= no_doorkey ||
       keyType >= max_doorkey ||
       d->doorKeys[keyType] == true ) {
    return;
  }

  d->doorKeys[keyType] = true;
}

void GameWorld::removeDoorKey( int keyType )
{
  if ( keyType <= no_doorkey ||
       keyType >= max_doorkey ||
       d->doorKeys[keyType] == false ) {
    return;
  }

  d->doorKeys[keyType] = false;
}

bool GameWorld::hasDoorKey( int keyType )
{
  if ( keyType <= no_doorkey || keyType >= max_doorkey ) {
    return false;
  }

  return d->doorKeys[keyType];
}

void GameWorld::setWorldTitle( const std::string &title )
{
  d->worldTitle = title;
}

const std::string &GameWorld::worldTitle() const
{
  return d->worldTitle;
}

void GameWorld::addGameFlag( const std::string &flag )
{
  if (hasGameFlag(flag)) {
    return;
  }

  d->gameFlags.push_back( flag );
}

void GameWorld::removeGameFlag( const std::string &flag )
{
  if (!hasGameFlag(flag)) {
    return;
  }

  d->gameFlags.remove( flag );
}

bool GameWorld::hasGameFlag( const std::string &flag )
{
  std::list<std::string>::iterator it;
  for ( it = d->gameFlags.begin(); it != d->gameFlags.end(); it++ )
  {
    const std::string &storedFlag = *it;
    if ( flag == storedFlag ) {
      return true;
    }
  }
  return false;
}

void GameWorld::addBoard( int index, GameBoard *board )
{
  board->setWorld(this);
  d->boards[index] = board;
  if ( d->maxBoards <= index ) {
    d->maxBoards = index + 1;
  };
}

GameBoard * GameWorld::getBoard( int index ) const
{
  GameBoardMap::iterator it;
  it = d->boards.find(index);
  if ( it == d->boards.end() ) {
    return 0;
  }

  return (*it).second;
}

int GameWorld::indexOf( GameBoard *board ) const
{
  GameBoardMap::iterator it;
  for ( it = d->boards.begin(); it != d->boards.end(); it++ ) {
    if ( (*it).second == board ) {
      return (*it).first;
    }
  }
  return -1;
}

int GameWorld::maxBoards() const
{
  return d->maxBoards;
}

void GameWorld::setCurrentBoard( GameBoard *board )
{
  d->currentBoard = board;
}

GameBoard * GameWorld::currentBoard() const
{
  return d->currentBoard;
}

void GameWorld::paint( AbstractPainter *painter )
{
  if (d->currentBoard) {
    d->currentBoard->paint( painter );
  }

  int x = d->currentTimePassed;
  
  std::ostringstream sstr;
  sstr.flags( std::ios::right );
  sstr.width( 5 );
  sstr << x;
  painter->drawText( 70, 2, 0x0f, sstr.str() );
}

