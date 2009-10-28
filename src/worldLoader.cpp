// Insert copyright and license information here.

#include <string>
#include <cstdio>
#include <cstdlib>
#include <memory>

#include "debug.h"
#include "gameWorld.h"
#include "gameBoard.h"
#include "worldLoader.h"
#include "entityManager.h"
#include "zztEntity.h"
#include "zztStructs.h"

class WorldLoaderPrivate
{
  public:
    WorldLoaderPrivate( WorldLoader *pSelf )
      : world( 0 ),
        self(pSelf)
    { /* */ };

    GameWorld *world;
    std::string filename;
    std::FILE *file;

    ZZT::WorldHeader header;

    GameBoard *loadBoard();
    bool readFieldDataRLE( GameBoard *board, int maxLen );
  
  private:
    WorldLoader *self;
};

GameBoard * WorldLoaderPrivate::loadBoard()
{
  std::auto_ptr<GameBoard > board( new GameBoard() );
  board->setWorld( world );
  board->clear();

  const long int filePos = std::ftell( file );

  // always load in heap, stack bad
  const std::auto_ptr<ZZT::BoardHeader> header( new ZZT::BoardHeader );
  int count = std::fread( header.get(), sizeof( ZZT::BoardHeader ), 1, file );

  if (!count) {
    zwarn() << "Failed to load header while loading board.";
    return 0;
  }

  zinfo() << "Board info: " << header->sizeInBytes << " "
                            << header->title.toStdString();

  std::fseek( file, filePos + 0x34, SEEK_SET );
  bool rleSuccess = readFieldDataRLE( board.get(),
          header->sizeInBytes - sizeof( ZZT::BoardHeader ) );

  if (!rleSuccess) {
    zwarn() << "Failed to load RLE while loading board.";
    return 0;
  }

  std::fseek( file, filePos + header->sizeInBytes + 2, SEEK_SET );
  return board.release();
}

bool WorldLoaderPrivate::readFieldDataRLE( GameBoard *board, int maxLen )
{
  int reps, id, color;

  int count = 0;
  int bytes = 0;

  while ( count < 1500 && bytes < maxLen )
  {
    reps = std::fgetc(file);
    id = std::fgetc(file);
    color = std::fgetc(file);
    bytes += 3;

    if ( reps == EOF || id == EOF || color == EOF ) {
      return false;
    }

    for ( int x = 0; x < reps; x++ ) {
      ZZTEntity *entity = world->entityManager()->createEntity( id, color );
      zdebug() << id << " " << color << " " << (entity ? entity->tile() : 0);
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
  d->filename = filename;
  d->file = std::fopen( filename.c_str(), "r+" );
  if (!d->file) {
    zwarn() << "WorldLoader: File load error";
  }
  else {
    zinfo() << "Loading world " << filename;
  }
}

WorldLoader::~WorldLoader()
{
  if (d->file) {
    std::fclose(d->file);
  }

  delete d;
  d = 0;
}

bool WorldLoader::isValid() const
{
  if (!d->file) {
    return false;
  }

  return true;
}

void WorldLoader::setWorld( GameWorld *target )
{
  d->world = target;
}

bool WorldLoader::go()
{
  if (!d->file) {
    return false;
  }

  int count = std::fread( &d->header, sizeof( ZZT::WorldHeader ), 1, d->file );

  if (!count) {
    zwarn() << "Error reading file " << std::ferror( d->file );
    return false;
  }

  zinfo() << "Magickey: " << d->header.magicKey;
  if ( d->header.magicKey != 0xffff ) {
    zwarn() << "Not a ZZT world file!";
    return false;
  }
 
  d->world->setWorldTitle( d->header.gameName.toStdString() );
  zinfo() << "GameName: " << d->world->worldTitle();

  // load counters
  d->world->setCurrentAmmo( d->header.ammo );
  d->world->setCurrentHealth( d->header.health );
  d->world->setCurrentGems( d->header.gems );
  d->world->setCurrentTorches( d->header.torches );
  d->world->setCurrentTorchCycles( d->header.torchCycles );
  d->world->setCurrentEnergizerCycles( d->header.energizerCycles );
  d->world->setCurrentTimePassed( d->header.time );

  // load keys
  if ( d->header.blueKey ) {
    d->world->addDoorKey( GameWorld::BLUE_DOORKEY );
  }

  if ( d->header.greenKey ) {
    d->world->addDoorKey( GameWorld::GREEN_DOORKEY );
  }

  if ( d->header.cyanKey ) {
    d->world->addDoorKey( GameWorld::CYAN_DOORKEY );
  }

  if ( d->header.redKey ) {
    d->world->addDoorKey( GameWorld::RED_DOORKEY );
  }

  if ( d->header.purpleKey ) {
    d->world->addDoorKey( GameWorld::PURPLE_DOORKEY );
  }

  if ( d->header.yellowKey ) {
    d->world->addDoorKey( GameWorld::YELLOW_DOORKEY );
  }

  if ( d->header.whiteKey ) {
    d->world->addDoorKey( GameWorld::WHITE_DOORKEY );
  }

  // load gameflags
  for ( int x = 0; x < 10; x++ ) {
    std::string str = d->header.flags[x].toStdString();
    if ( !str.empty() ) {
      d->world->addGameFlag( str );
      zinfo() << "Flag: " << str;
    }
  }

  int boards = d->header.boardCount + 1;
  zinfo() << "Adding boards: " << boards;

  // debugging
  boards = 1;

  for ( int x = 0; x < boards; x++ ) {
    GameBoard *board = d->loadBoard();
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

