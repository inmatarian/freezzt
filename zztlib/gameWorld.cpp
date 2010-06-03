/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <string>
#include <map>
#include <list>
#include <sstream>

#include "debug.h"
#include "zstring.h"
#include "defines.h"
#include "gameWorld.h"
#include "gameBoard.h"
#include "abstractPainter.h"
#include "abstractMusicStream.h"

enum { BOARD_SWITCH_NONE = -1 };
typedef std::map<int, GameBoard*> GameBoardMap;

// ---------------------------------------------------------------------------

class GameWorldPrivate
{
  public:
    GameWorldPrivate( GameWorld *pSelf );
    virtual ~GameWorldPrivate();

    int startBoard;
    int currentAmmo;
    int currentGems;
    int currentHealth;
    int currentTorches;
    int currentTorchCycles;
    int currentScore;
    int currentEnergizerCycles;
    int currentTimePassed;
    bool doorKeys[ GameWorld::max_doorkey ];

    // keyboard presses from the player
    bool pressed_up;
    bool pressed_down;
    bool pressed_left;
    bool pressed_right;
    bool pressed_shoot_up;
    bool pressed_shoot_down;
    bool pressed_shoot_left;
    bool pressed_shoot_right;
    bool pressed_torch;

    std::string worldTitle;
    std::list<std::string> gameFlags;

    GameBoardMap boards;
    int maxBoards;
    GameBoard *currentBoard;

    int boardSwitch;

    int cycleCountdown;
    int cycleSetting;

    int transitionCount;
    bool *transitionTiles;

    AbstractMusicStream *musicStream;

  private:
    GameWorld *self;
};

GameWorldPrivate::GameWorldPrivate( GameWorld *pSelf )
  : startBoard( 0 ),
    currentAmmo( 0 ),
    currentGems( 0 ),
    currentHealth( 0 ),
    currentTorches( 0 ),
    currentTorchCycles( 0 ),
    currentScore( 0 ),
    currentEnergizerCycles( 0 ),
    currentTimePassed( 0 ),
    pressed_up( false ),
    pressed_down( false ),
    pressed_left( false ),
    pressed_right( false ),
    pressed_shoot_up( false ),
    pressed_shoot_down( false ),
    pressed_shoot_left( false ),
    pressed_shoot_right( false ),
    pressed_torch( false ),
    maxBoards( 0 ),
    currentBoard( 0 ),
    boardSwitch( BOARD_SWITCH_NONE ),
    cycleCountdown( 0 ),
    cycleSetting( 4 ),
    transitionCount( 0 ),
    transitionTiles( 0 ),
    musicStream( 0 ),
    self(pSelf)
{
  for ( int x = GameWorld::BLUE_DOORKEY; x < GameWorld::max_doorkey; x++ ) {
    doorKeys[x] = false;
  }

  transitionTiles = new bool[1500];
  for ( int i = 0; i<1500; i++ ) {
    transitionTiles[i] = false;
  }
}

GameWorldPrivate::~GameWorldPrivate()
{
  // boards need to clean up their entities, they go first
  GameBoardMap::iterator iter;
  for( iter = boards.begin(); iter != boards.end(); ++iter ) {
    delete (*iter).second;
  }

  delete[] transitionTiles;

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

void GameWorld::setStartBoard( int index )
{
  d->startBoard = index;
}

int GameWorld::startBoard() const
{
  return d->startBoard;
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

void GameWorld::update()
{
  if ( d->cycleCountdown > 0 ) {
    d->cycleCountdown -= 1;
  }
  else {
    d->cycleCountdown = d->cycleSetting;
    exec();
  }
}

void GameWorld::exec()
{
  d->musicStream->begin();
  if ( d->boardSwitch != BOARD_SWITCH_NONE ) {
    setCurrentBoard( getBoard(d->boardSwitch) );
    d->boardSwitch = BOARD_SWITCH_NONE;
  }

  if (d->currentTorchCycles > 0) {
    d->currentTorchCycles -= 1;
  }

  if ( d->pressed_torch &&
       d->currentTorchCycles == 0 &&
       d->currentTorches > 0 ) {
    d->currentTorches -= 1;
    d->currentTorchCycles = 1000;
  }

  if (d->currentEnergizerCycles > 0) {
    d->currentTorchCycles -= 1;
  }

  currentBoard()->exec();
  clearInputKeys();
  d->musicStream->end();
}

void GameWorld::paint( AbstractPainter *painter )
{
  if (d->currentBoard) {
    d->currentBoard->paint( painter );
  }

  if ( d->transitionCount > 0 )
  {
    // paint transitions
    for ( int i = 0; i<1500; i++ )
    {
      if ( !d->transitionTiles[i] ) {
        continue;
      }

      const int x = (i%60);
      const int y = (i/60);
      painter->paintChar( x, y, 0xdb, Defines::DARK_PURPLE );
    }
  }
}

void GameWorld::setTransitionTile( int x, int y, bool on )
{
  if ( x < 0 || x >= 60 || y < 0 || y >= 25 ) {
    return;
  }

  const int index = y*60 + x;

  if ( d->transitionTiles[index] != on ) {
    d->transitionTiles[index] = on;
    d->transitionCount += on ? 1 : -1;
  }
}

bool GameWorld::transitionTile( int x, int y ) const
{
  if ( x < 0 || x >= 60 || y < 0 || y >= 25 ) {
    return false;
  }

  const int index = y*60 + x;
  return d->transitionTiles[index];
}

void GameWorld::addInputKey( int keycode, int unicode )
{
  using namespace Defines;
  switch ( keycode )
  {
    case Z_Unicode:
      switch ( unicode )
      {
        case '[':
        case ']': {
          // level changing debug keys
          GameBoard *board = currentBoard();
          int index = indexOf(board);
          index += ( unicode == '[' ) ? -1 : 1;
          if ( index < 0 || index >= maxBoards() ) break;
          changeActiveBoard(index);
          break;
        }

        case 't':
        case 'T':
          d->pressed_torch = true;
          break;
      }    
      break;

    case Z_Up:         d->pressed_up = true; break;
    case Z_Down:       d->pressed_down = true; break;
    case Z_Left:       d->pressed_left = true; break;
    case Z_Right:      d->pressed_right = true; break;
    case Z_ShootUp:    d->pressed_shoot_up = true; break;
    case Z_ShootDown:  d->pressed_shoot_down = true; break;
    case Z_ShootLeft:  d->pressed_shoot_left = true; break;
    case Z_ShootRight: d->pressed_shoot_right = true; break;

    default: break;
  }
}

void GameWorld::clearInputKeys()
{
  d->pressed_up = false;
  d->pressed_down = false;
  d->pressed_left = false;
  d->pressed_right = false;
  d->pressed_shoot_up = false;
  d->pressed_shoot_down = false;
  d->pressed_shoot_left = false;
  d->pressed_shoot_right = false;
  d->pressed_torch = false;
}

bool GameWorld::upPressed() const
{
  return d->pressed_up;
}

bool GameWorld::downPressed() const
{
  return d->pressed_down;
}

bool GameWorld::leftPressed() const
{
  return d->pressed_left;
}

bool GameWorld::rightPressed() const
{
  return d->pressed_right;
}

bool GameWorld::shootUpPressed() const
{
  return d->pressed_shoot_up;
}

bool GameWorld::shootDownPressed() const
{
  return d->pressed_shoot_down;
}

bool GameWorld::shootLeftPressed() const
{
  return d->pressed_shoot_left;
}

bool GameWorld::shootRightPressed() const
{
  return d->pressed_shoot_right;
}

void GameWorld::changeActiveBoard( int index )
{
  d->boardSwitch = index;
}

void GameWorld::setMusicStream( AbstractMusicStream *musicStream )
{
  d->musicStream = musicStream;
}

AbstractMusicStream *GameWorld::musicStream() const
{
  return d->musicStream;
}

void GameWorld::setFrameCycle( int setting )
{
  d->cycleSetting = setting;
}

