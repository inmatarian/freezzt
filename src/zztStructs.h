// Insert copyright and license information here.

#ifndef ZZT_STRUCTS_H
#define ZZT_STRUCTS_H

// ---------------------------------------------------------------------------
// kev's file format

namespace ZZT {

template <int t>
struct String
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
  ZZT::String<20> gameName;
  ZZT::String<20> flags[10];
  unsigned short time;
  unsigned short blank_second;
  unsigned char savegame;
  unsigned char blank_third[247];
}
__attribute__((__packed__));

struct BoardHeader
{
  signed short sizeInBytes;
  ZZT::String<33> title;
  unsigned char blank[16];
}
__attribute__((__packed__));

struct BoardInformation
{
  unsigned char maximumShotsFired;
  unsigned char darkness;
  unsigned char boardNorth;
  unsigned char boardSouth;
  unsigned char boardWest;
  unsigned char boardEast;
  unsigned char reenterZapped;
  ZZT::String<58> message;
  unsigned char enterX;
  unsigned char enterY;
  signed short timeLimit;
  unsigned char padding[16];
  signed short thingCount;
}
__attribute__((__packed__));

}; // namespace ZZT

#endif // ZZT_STRUCTS_H

