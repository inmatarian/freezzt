// Insert copyright and license information here.

#include <string>
#include <cstdio>
#include <cstdlib>

#include "debug.h"
#include "gameWorld.h"
#include "gameBoard.h"
#include "worldLoader.h"

// ---------------------------------------------------------------------------
// kev's file format

template <int t>
struct zztstring
{
  unsigned char length;
  char name[t];

  std::string toStdString() const
  {
    int len = ( length <= t
                ? length
                : 0 );
    char str[t+1];

    for ( int x = 0; x < len; x++ ) {
      str[x] = name[x];
    }
    str[len] = 0;

    return std::string(str);    
  };
}
__attribute__((__packed__));

struct WorldHeader
{
  unsigned short magicKey;   // FFFFh
  signed short boardCount; // minus 1, so 0x0 is 1, 0x1 is 2, etc.
  signed short ammo;
  signed short gems;
  unsigned char blueKey;
  unsigned char greenKey;
  unsigned char cyanKey;
  unsigned char redKey;
  unsigned char purpleKey;
  unsigned char yellowKey;
  unsigned char whiteKey;
  signed short health;
  signed short startBoard;
  signed short torches;
  signed short torchCycles;
  signed short energizerCycles;
  unsigned short blank_first;
  signed short score;
  zztstring<20> gameName;
  zztstring<20> flags[10];
  unsigned short time;
  unsigned short blank_second;
  unsigned char savegame;
  unsigned char blank_third[247];
}
__attribute__((__packed__));

// ---------------------------------------------------------------------------

class WorldLoaderPrivate
{
  public:
    GameWorld *world;
    std::string filename;
    std::FILE *file;

    WorldHeader header;
};

WorldLoader::WorldLoader( const std::string &filename )
  : d( new WorldLoaderPrivate() )
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

  int count;
  count = std::fread( &d->header, sizeof( WorldHeader ), 1, d->file );

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
  for ( int x = 0; x < boards; x++ ) {
    d->world->addBoard( x, new GameBoard() );
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

