/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <string>
#include <cstring>
#include <cstdlib>
#include <memory>
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

#include "debug.h"
#include "gameWorld.h"
#include "gameBoard.h"
#include "thingFactory.h"
#include "worldLoader.h"
#include "zztEntity.h"
#include "zztStructs.h"

class WorldLoaderPrivate
{
  public:
    WorldLoaderPrivate( WorldLoader *pSelf )
      : world( 0 ),
        worldData( 0 ),
        self(pSelf)
    { /* */ };

    GameBoard *loadBoard( int &filePos );
    bool readFieldDataRLE( GameBoard *board,
                           int &filePos,
                           int failSafeStopPos );

  public:
    GameWorld *world;
    std::string filename;

    unsigned char *worldData;
    int fileSize;
  
    ThingFactory thingFactory;

  private:
    WorldLoader *self;
};

GameBoard * WorldLoaderPrivate::loadBoard( int &filePos )
{
  std::auto_ptr<GameBoard > board( new GameBoard() );
  board->setWorld( world );
  board->clear();

  std::auto_ptr<BoardHeader>
      header( new BoardHeader(worldData + filePos) );

  const int origFilePos = filePos;
  const int endFilePos = filePos + header->sizeInBytes + 2;

  zinfo() << "Board header: "
          << header->sizeInBytes << " "
          << origFilePos << " "
          << endFilePos << " "
          << header->title;

  filePos += 0x35;

  bool rleSuccess = readFieldDataRLE( board.get(), filePos, endFilePos );
  if (!rleSuccess) {
    zwarn() << "Failed to load RLE while loading board.";
    return 0;
  }

  std::auto_ptr<BoardInformation>
      info( new BoardInformation(worldData + filePos) );

  zinfo() << "Board info: "
          << info->thingCount << " "
          << info->message;

  board->setMessage( info->message );
  board->setNorthExit( info->boardNorth );
  board->setSouthExit( info->boardSouth );
  board->setWestExit( info->boardWest );
  board->setEastExit( info->boardEast );

  filePos += 0x58;

  thingFactory.setBoard( board.get() );

  for ( int x = 0; x < info->thingCount + 1; x++ )
  {
    int thingSize = 0;
    ZZTThing *thing = thingFactory.createThing( worldData + filePos, thingSize );
    if (thing) {
      board->addThing( thing );
    }
    filePos += thingSize;
  }

  zdebug() << "End of board file pos: " << filePos;

  filePos = endFilePos;
  return board.release();
}

bool WorldLoaderPrivate::readFieldDataRLE( GameBoard *board,
                                           int &filePos,
                                           int failSafeStopPos )
{
  int reps, id, color;
  int count = 0;

  while ( count < 1500 && filePos < failSafeStopPos )
  {
    reps = (int) worldData[filePos++];
    id = (int) worldData[filePos++];
    color = (int) worldData[filePos++];

    for ( int x = 0; x < reps && count < 1500; x++ ) {
      ZZTEntity entity = ZZTEntity::createEntity( id, color );
      board->setEntity( count % 60, count / 60, entity );
      count += 1;
    }
  }
  return true;
}

// ---------------------------------------------------------------------------

WorldLoader::WorldLoader( const std::string &filename )
  : d( new WorldLoaderPrivate(this) )
{
  using namespace std;

  d->filename = filename;

  ifstream file( filename.c_str(), ios::in|ios::binary|ios::ate );
  if ( file.is_open() && file.good() ) {
    d->fileSize = file.tellg();
    d->worldData = new unsigned char[d->fileSize];
    file.seekg( 0, ios::beg );
    file.read( (char*)d->worldData, d->fileSize );
    file.close();
  }

  if ( !isValid() ) {
    zwarn() << "WorldLoader: File load error";
  }
  else {
    zinfo() << "Loading world " << filename;
  }
}

WorldLoader::~WorldLoader()
{
  if (d->worldData) {
    delete d->worldData;
  }

  delete d;
  d = 0;
}

bool WorldLoader::isValid() const
{
  return (d->worldData);
}

void WorldLoader::setWorld( GameWorld *target )
{
  d->world = target;
}

bool WorldLoader::go()
{
  if ( !isValid() ) {
    return false;
  }

  d->thingFactory.setWorld( d->world );

  int filePos = 0;

  std::auto_ptr<WorldHeader>
      header( new WorldHeader(d->worldData) );

  zinfo() << "Magickey: " << header->magicKey;
  if ( header->magicKey != 0xffff ) {
    zwarn() << "Not a ZZT world file!";
    return false;
  }
 
  d->world->setWorldTitle( header->gameName );
  zinfo() << "GameName: " << d->world->worldTitle();

  // load counters
  d->world->setStartBoard( header->startBoard );
  d->world->setCurrentAmmo( header->ammo );
  d->world->setCurrentHealth( header->health );
  d->world->setCurrentGems( header->gems );
  d->world->setCurrentTorches( header->torches );
  d->world->setCurrentTorchCycles( header->torchCycles );
  d->world->setCurrentEnergizerCycles( header->energizerCycles );
  d->world->setCurrentTimePassed( header->time );

  // load keys
  if ( header->blueKey ) {
    d->world->addDoorKey( GameWorld::BLUE_DOORKEY );
  }

  if ( header->greenKey ) {
    d->world->addDoorKey( GameWorld::GREEN_DOORKEY );
  }

  if ( header->cyanKey ) {
    d->world->addDoorKey( GameWorld::CYAN_DOORKEY );
  }

  if ( header->redKey ) {
    d->world->addDoorKey( GameWorld::RED_DOORKEY );
  }

  if ( header->purpleKey ) {
    d->world->addDoorKey( GameWorld::PURPLE_DOORKEY );
  }

  if ( header->yellowKey ) {
    d->world->addDoorKey( GameWorld::YELLOW_DOORKEY );
  }

  if ( header->whiteKey ) {
    d->world->addDoorKey( GameWorld::WHITE_DOORKEY );
  }

  // load gameflags
  for ( int x = 0; x < 10; x++ ) {
    std::string str = header->flags[x];
    if ( !str.empty() ) {
      d->world->addGameFlag( str );
      zinfo() << "Flag: " << str;
    }
  }

  int boards = header->boardCount + 1;
  zinfo() << "Adding boards: " << boards;
  filePos += 0x200;

  for ( int x = 0; x < boards; x++ ) {
    GameBoard *board = d->loadBoard(filePos);
    if (!board) {
      zwarn() << "Error loading world.";
      return false;
    }
    d->world->addBoard( x, board );
  }

  return true;
}

// ---------------------------------------------------------------------------

GameWorld * WorldLoader::loadWorld( const std::string &filename )
{
  WorldLoader loader( filename );
  if (!loader.isValid()) {
    return 0;
  }

  GameWorld *world = new GameWorld();
  loader.setWorld(world);

  bool success = loader.go();
  if (!success) {
    delete world;
    return 0;
  }

  return world;
}

