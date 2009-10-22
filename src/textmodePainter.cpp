// Insert copyright and license information here.

#include <iostream>
#include <SDL.h>

#include "debug.h"
#include "abstractPainter.h"
#include "textmodePainter.h"
#include "page437.h"

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

    void putPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
    {
      blitter->putpixel( surface->pixels,
                         surface->format->BytesPerPixel,
                         surface->pitch,
                         x,
                         y,
                         pixel );
    }
};

// ---------------------------------------------------------------------------

TextmodePainter::TextmodePainter()
  : d( new TextmodePainterPrivate() )
{
  /* */
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

  const unsigned char *tile = Page437::get_8x14_char(c);
  Uint32 forecolor = d->colors[ color&15 ];
  Uint32 backcolor = d->colors[ (color>>4)&7 ];

  unsigned char pixel;

  for ( int ty = 0; ty < 14; ty++ ) {
    for ( int tx = 0; tx < 8; tx++ ) {
      pixel = tile[ (8*ty) + tx ];
      d->putPixel( d->surface, (x*8)+tx, (y*14)+ty,
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
}

