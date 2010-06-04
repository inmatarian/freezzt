
#include <string>
#include "zstring.h"
#include "zztStructs.h"

// ---------------------------------------------------------------------------
// utility functions, define as needed for platform endianness

// zzt binary to little endian
unsigned char zztByte( const unsigned char *data )
{
  return *data;
}

// zzt binary to little endian
signed short zztWord( const unsigned char *data )
{
  return *(reinterpret_cast<const signed short*>(data));
}

// zzt binary to little endian
unsigned long zztLong( const unsigned char *data )
{
  return *(reinterpret_cast<const unsigned int*>(data));
}

// zzt binary to little endian
ZString zztString( const unsigned char *data )
{
  int len = *data;
  if (len > 127) len = 127; // minor security detail
  return ZString( (const char *)(data + 0x01), len );
}

// ---------------------------------------------------------------------------
// taken mostly from kev's file format

WorldHeader::WorldHeader( const unsigned char *data )
{
  magicKey = (unsigned short) zztWord( data + 0x00 );
  boardCount = zztWord( data + 0x02 );
  ammo = zztWord( data + 0x04 );
  gems = zztWord( data + 0x06 );

  blueKey = zztByte( data + 0x08 );
  greenKey = zztByte( data + 0x09 );
  cyanKey = zztByte( data + 0x0A );
  redKey = zztByte( data + 0x0B );
  purpleKey = zztByte( data + 0x0C );
  yellowKey = zztByte( data + 0x0D );
  whiteKey = zztByte( data + 0x0E );

  health = zztWord( data + 0x0F );
  startBoard = zztWord( data + 0x11 );
  torches = zztWord( data + 0x13 );
  torchCycles = zztWord( data + 0x15 );
  energizerCycles = zztWord( data + 0x17 );
  score = zztWord( data + 0x1B );

  gameName = zztString( data + 0x1D );

  for ( int x = 0; x < 10; x++ ) {
    flags[x] = zztString( data + (x * 21) + 0x32 );
  }

  time = zztWord( data + 0x104 );
  savegame = zztByte( data + 0x108 );
}

// ---------------------------------------------------------------------------

BoardHeader::BoardHeader( const unsigned char *data )
{
  sizeInBytes = zztWord( data );
  title = zztString( data + 0x02 );
}

// ---------------------------------------------------------------------------

BoardInformation::BoardInformation( const unsigned char *data )
{
  maximumShotsFired = zztByte( data + 0x00 );
  darkness = zztByte( data + 0x01 );
  boardNorth = zztByte( data + 0x02 );
  boardSouth = zztByte( data + 0x03 );
  boardWest = zztByte( data + 0x04 );
  boardEast = zztByte( data + 0x05 );
  reenterZapped = zztByte( data + 0x06 );

  message = zztString( data + 0x07 );

  enterX = zztByte( data + 0x42 );
  enterY = zztByte( data + 0x43 );
  timeLimit = zztWord( data + 0x44 );
  thingCount = zztWord( data + 0x56 );
}

// ---------------------------------------------------------------------------

ThingHeader::ThingHeader( const unsigned char *data )
{
  x = zztByte( data + 0x00 );
  y = zztByte( data + 0x01 );
  x_step = zztWord( data + 0x02 );
  y_step = zztWord( data + 0x04 );
  cycle = zztWord( data + 0x06 );
  param1 = zztByte( data + 0x08 );
  param2 = zztByte( data + 0x09 );
  param3 = zztByte( data + 0x0A );
  param4 = zztLong( data + 0x0B );
  underTile = zztByte( data + 0x0F );
  underColor = zztByte( data + 0x10 );
  currentInstruction = zztWord( data + 0x15 );
  programLength = zztWord( data + 0x17 );
}

