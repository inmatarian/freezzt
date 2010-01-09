/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <iostream>
#include <SDL.h>

#include "debug.h"
#include "abstractPainter.h"
#include "textmodePainter.h"

#include "page437_8x16.xbm"

using namespace std;

class AbstractPutPixel
{
  public:
    virtual void putpixel(void *pixels, int bpp, int pitch, int x, int y, Uint32 pixel)
    { /* */ };
};

class PutPixel_8 : public AbstractPutPixel
{
  public:
    virtual void putpixel(void *pixels, int bpp, int pitch, int x, int y, Uint32 pixel)
    {
      Uint8 *p = (Uint8 *)pixels + y * pitch + x * bpp;
      *p = pixel;
    }
};

class PutPixel_16 : public AbstractPutPixel
{
  public:
    virtual void putpixel(void *pixels, int bpp, int pitch, int x, int y, Uint32 pixel)
    {
      Uint8 *p = (Uint8 *)pixels + y * pitch + x * bpp;
      *(Uint16 *)p = pixel;
    }
};

class PutPixel_24_LittleEndian : public AbstractPutPixel
{
  public:
    virtual void putpixel(void *pixels, int bpp, int pitch, int x, int y, Uint32 pixel)
    {
      Uint8 *p = (Uint8 *)pixels + y * pitch + x * bpp;
      p[0] = pixel & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = (pixel >> 16) & 0xff;
    }
};

class PutPixel_24_BigEndian : public AbstractPutPixel
{
  public:
    virtual void putpixel(void *pixels, int bpp, int pitch, int x, int y, Uint32 pixel)
    {
      Uint8 *p = (Uint8 *)pixels + y * pitch + x * bpp;
      p[0] = (pixel >> 16) & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = pixel & 0xff;
    }
};

class PutPixel_32 : public AbstractPutPixel
{
  public:
    virtual void putpixel(void *pixels, int bpp, int pitch, int x, int y, Uint32 pixel)
    {
      Uint8 *p = (Uint8 *)pixels + y * pitch + x * bpp;
      *(Uint32 *)p = pixel;
    }
};

// ---------------------------------------------------------------------------

class TextmodePainterPrivate
{
  public:
    TextmodePainterPrivate() : surface(0) { /* */ };

    Uint32 colors[16];

    SDL_Surface *surface;
    AbstractPutPixel *blitter;

    AbstractPutPixel          putPixel_dummy;
    PutPixel_8                putPixel8;
    PutPixel_16               putPixel16;
    PutPixel_24_LittleEndian  putPixel24_littleEndian;
    PutPixel_24_BigEndian     putPixel24_bigEndian;
    PutPixel_32               putPixel32;

    unsigned short dirtyMap[25][80];

    void putPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
    {
      blitter->putpixel( surface->pixels,
                         surface->format->BytesPerPixel,
                         surface->pitch,
                         x,
                         y,
                         pixel );
    }

    unsigned char pixelRow( unsigned char c, int row ) const
    {
      const int pitch = page437_8x16_width / 8;
      const int volume = page437_8x16_height / 16 * pitch;

      const int target = ( row * pitch ) + ( c / 16 * volume ) + ( c % 16 );

      return page437_8x16_bits[target];
    }
};

// ---------------------------------------------------------------------------

TextmodePainter::TextmodePainter()
  : d( new TextmodePainterPrivate() )
{
  for ( int y = 0; y < 25; y++ ) {
    for ( int x = 0; x < 80; x++ ) {
      d->dirtyMap[y][x] = 0;
    }
  }
}

TextmodePainter::~TextmodePainter()
{
  delete d;
  d = 0;
}

void TextmodePainter::setSDLSurface( SDL_Surface *surface )
{
  d->surface = surface;
}

void TextmodePainter::begin()
{
  if ( SDL_MUSTLOCK(d->surface) ) {
    if ( SDL_LockSurface(d->surface) < 0 ) {
      zwarn() << "Couldn't lock surface" << endl;
    }
  }

  d->colors[ 0] = SDL_MapRGB( d->surface->format, 0x00, 0x00, 0x00 );
  d->colors[ 1] = SDL_MapRGB( d->surface->format, 0x00, 0x00, 0xaa );
  d->colors[ 2] = SDL_MapRGB( d->surface->format, 0x00, 0xaa, 0x00 );
  d->colors[ 3] = SDL_MapRGB( d->surface->format, 0x00, 0xaa, 0xaa );
  d->colors[ 4] = SDL_MapRGB( d->surface->format, 0xaa, 0x00, 0x00 );
  d->colors[ 5] = SDL_MapRGB( d->surface->format, 0xaa, 0x00, 0xaa );
  d->colors[ 6] = SDL_MapRGB( d->surface->format, 0xaa, 0x55, 0x00 );
  d->colors[ 7] = SDL_MapRGB( d->surface->format, 0xaa, 0xaa, 0xaa );
  d->colors[ 8] = SDL_MapRGB( d->surface->format, 0x55, 0x55, 0x55 );
  d->colors[ 9] = SDL_MapRGB( d->surface->format, 0x55, 0x55, 0xff );
  d->colors[10] = SDL_MapRGB( d->surface->format, 0x55, 0xff, 0x55 );
  d->colors[11] = SDL_MapRGB( d->surface->format, 0x55, 0xff, 0xff );
  d->colors[12] = SDL_MapRGB( d->surface->format, 0xff, 0x55, 0x55 );
  d->colors[13] = SDL_MapRGB( d->surface->format, 0xff, 0x55, 0xff );
  d->colors[14] = SDL_MapRGB( d->surface->format, 0xff, 0xff, 0x55 );
  d->colors[15] = SDL_MapRGB( d->surface->format, 0xff, 0xff, 0xff );

  int bpp = d->surface->format->BytesPerPixel;
  switch (bpp) {
    case 1: d->blitter = &d->putPixel8; break;
    case 2: d->blitter = &d->putPixel16; break;
    case 3:
      d->blitter = ( SDL_BYTEORDER == SDL_BIG_ENDIAN
                     ? (AbstractPutPixel*) &d->putPixel24_bigEndian
                     : (AbstractPutPixel*) &d->putPixel24_littleEndian );
      break;
    case 4: d->blitter = &d->putPixel32; break;
    default: d->blitter = &d->putPixel_dummy; break;
  }
}

void TextmodePainter::paintChar( int x, int y, unsigned char c, unsigned char color )
{
  if (!d->surface || !d->blitter) {
    zwarn() << "Attempted to paint without a begin called" << endl;
    return;
  }

  unsigned short dirtyKey = ( color << 8 ) + c;
  if ( d->dirtyMap[y][x] == c ) {
    // no need to redraw this
    return;
  }

  d->dirtyMap[y][x] = dirtyKey;

  Uint32 forecolor = d->colors[ color&15 ];
  Uint32 backcolor = d->colors[ (color>>4)&7 ];

  for ( int ty = 0; ty < 16; ty++ )
  {
    const int row = d->pixelRow( c, ty );

    for ( int tx = 0; tx < 8; tx++ )
    {
      const int pixel = 1 - ( ( row >> tx ) & 1 );
      d->putPixel( d->surface, (x*8)+tx, (y*16)+ty,
                   ( pixel ? forecolor : backcolor ) );
    }
  }
}

void TextmodePainter::end()
{
  if ( SDL_MUSTLOCK(d->surface) ) {
    SDL_UnlockSurface(d->surface);
  }

  d->blitter = 0;
  SDL_Flip( d->surface );
}

