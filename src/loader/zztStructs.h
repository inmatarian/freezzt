// Insert copyright and license information here.

#ifndef ZZT_STRUCTS_H
#define ZZT_STRUCTS_H

struct WorldHeader
{
  WorldHeader( const unsigned char *data );

  unsigned short magicKey; // FFFFh
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
  signed short score;
  std::string gameName;
  std::string flags[10];
  unsigned short time;
  unsigned char savegame;
};

struct BoardHeader
{
  BoardHeader( const unsigned char *data );

  signed short sizeInBytes;
  std::string title;
};

struct BoardInformation
{
  BoardInformation( const unsigned char *data );

  unsigned char maximumShotsFired;
  unsigned char darkness;
  unsigned char boardNorth;
  unsigned char boardSouth;
  unsigned char boardWest;
  unsigned char boardEast;
  unsigned char reenterZapped;
  std::string message;
  unsigned char enterX;
  unsigned char enterY;
  signed short timeLimit;
  signed short thingCount;
};

#endif // ZZT_STRUCTS_H

