/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <iostream>
#include <SDL.h>

#include "debug.h"
#include "zstring.h"
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
    TextmodePainterPrivate()
      : surface(0),
        blitter(0)
    { /* */ };

    Uint32 colors[16];

    SDL_Surface *surface;
    AbstractPutPixel *blitter;

    AbstractPutPixel          putPixel_dummy;
    PutPixel_8                putPixel8;
    PutPixel_16               putPixel16;
    PutPixel_24_LittleEndian  putPixel24_littleEndian;
    PutPixel_24_BigEndian     putPixel24_bigEndian;
    PutPixel_32               putPixel32;

    int dirtyMap[25][80];
    int cleanMap[25][80];

  public:
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

    void paintChar( int x, int y, unsigned char c, unsigned char color,
                    bool blinkOn, int offsetX, int offsetY )
    {
      const bool blinking = (color >> 7) && blinkOn;
      Uint32 forecolor = colors[ color&15 ];
      Uint32 backcolor = colors[ (color>>4)&7 ];

      // form block that the char will be drawn in.
      const int lx = offsetX + (x*8);
      const int rx = offsetX + (x*8) + 7;
      const int uy = offsetY + (y*16);
      const int dy = offsetY + (y*16) + 15;

      // make sure the block isn't entirely outside the window
      if ( lx >= surface->w || rx < 0 || uy >= surface->h || dy < 0 ) {
        return;
      }

      // establish valid pixel drawing ranges, so we don't segfault.
      int sx = ( lx < 0 ) ? -lx : 0;
      int sy = ( uy < 0 ) ? -uy : 0;
      int ex = ( rx >= surface->w ) ? 8 - (rx - surface->w) : 8;
      int ey = ( dy >= surface->h ) ? 16 - (dy - surface->h) : 16;

      for ( int ty = sy; ty < ey; ty++ )
      {
        const int row = pixelRow( c, ty );

        for ( int tx = sx; tx < ex; tx++ )
        {
          const int pixel = blinking ? 0 : ( 1 - ( ( row >> tx ) & 1 ));
          putPixel( surface, offsetX+(x*8)+tx, offsetY+(y*16)+ty,
                    ( pixel ? forecolor : backcolor ) );
        }
      }
    }

    void determineCentering( int &offsetX, int &offsetY )
    {
      const int gameWidth = 80 * 8;
      const int gameHeight = 25 * 16;
      const int screenWidth = surface->w;
      const int screenHeight = surface->h;
      offsetX = - ((gameWidth - screenWidth) / 2);
      offsetY = - ((gameHeight - screenHeight) / 2);
    }
};

// ---------------------------------------------------------------------------

TextmodePainter::TextmodePainter()
  : AbstractPainter(),
    d( new TextmodePainterPrivate() )
{
  for ( int y = 0; y < 25; y++ ) {
    for ( int x = 0; x < 80; x++ ) {
      d->dirtyMap[y][x] = 0;
      d->cleanMap[y][x] = 0;
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
  for ( int y = 0; y < 25; y++ ) {
    for ( int x = 0; x < 80; x++ ) {
      d->cleanMap[y][x] = 0;
    }
  }
}

void TextmodePainter::begin_impl()
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
  int mapKey = c + ( color << 8 ) + (blinkOn() ? 1 << 16 : 0);
  d->dirtyMap[y][x] = mapKey;
}

void TextmodePainter::end_impl()
{
  if (!d->surface || !d->blitter) {
    zwarn() << "Attempted to paint without a begin called" << endl;
    return;
  }

  bool needFlip = false;

  
  int offsetX = 0;
  int offsetY = 0;
  d->determineCentering( offsetX, offsetY );

  for ( int y = 0; y < 25; y++ )
  {
    for ( int x = 0; x < 80; x++ )
    {
      int mapKey = d->dirtyMap[y][x];
      if ( mapKey == d->cleanMap[y][x] ) {
        // no need to redraw this
        continue;
      }
        
      needFlip = true;
      d->cleanMap[y][x] = mapKey;
      int color = ( mapKey >> 8 );
      int c = ( mapKey & 0xff );

      d->paintChar( x, y, c, color, blinkOn(), offsetX, offsetY );
    }
  }

  if ( SDL_MUSTLOCK(d->surface) ) {
    SDL_UnlockSurface(d->surface);
  }

  d->blitter = 0;

  if (needFlip) {
    SDL_Flip( d->surface );
  }
}

int TextmodePainter::currentTime()
{
  return SDL_GetTicks();
}

