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
#include "simplePainter.h"

#include "page437_8x16.xbm"

using namespace std;

// ---------------------------------------------------------------------------

static unsigned char pixelRow( unsigned char c, int row )
{
  const int pitch = page437_8x16_width / 8;
  const int volume = page437_8x16_height / 16 * pitch;

  const int target = ( row * pitch ) + ( c / 16 * volume ) + ( c % 16 );

  return page437_8x16_bits[target];
}

// ---------------------------------------------------------------------------

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

struct Cell
{
  int x;
  int y;
  int w;
  int h;

  unsigned char color;
  unsigned char character;
  bool blinkOn;

  bool dirty;

  Cell( int px = 0, int py = 0, int pw = 0, int ph = 0 )
    : x(px), y(py), w(pw), h(ph), color(0), character(0), blinkOn(false), dirty(true)
    { /* */ }; 

  bool isClean() const { return !dirty; };
  void makeClean() { dirty = false; };
  void makeDirty( unsigned char pcharacter, unsigned char pcolor, bool pblink )
  {
    if ( color != pcolor ) dirty = true;
    if ( pcharacter != character ) dirty = true;
    if ( pblink != blinkOn && (color & 0x80) ) dirty = true;
    color = pcolor;
    character = pcharacter;
    blinkOn = pblink; 
  }
};

// ---------------------------------------------------------------------------

class SimplePainterPrivate
{
  public:
    SimplePainterPrivate()
      : surface(0),
        blitter(0),
        fitSurface(true)
    { /* */ };

    void putPixel(int x, int y, Uint32 pixel);

    void paintRow( int x, int y, int width, unsigned char row,
                   Uint32 forecolor, Uint32 backcolor );

    void paintChar( int x, int y, unsigned char c, unsigned char color, bool blinkOn );

    void calculateCells();

  public:
    Uint32 colors[16];

    SDL_Surface *surface;
    AbstractPutPixel *blitter;

    bool fitSurface;

    AbstractPutPixel          putPixel_dummy;
    PutPixel_8                putPixel8;
    PutPixel_16               putPixel16;
    PutPixel_24_LittleEndian  putPixel24_littleEndian;
    PutPixel_24_BigEndian     putPixel24_bigEndian;
    PutPixel_32               putPixel32;

    Cell cells[25][80];
};

void SimplePainterPrivate::putPixel(int x, int y, Uint32 pixel)
{
  blitter->putpixel( surface->pixels,
                     surface->format->BytesPerPixel,
                     surface->pitch,
                     x,
                     y,
                     pixel );
}

void SimplePainterPrivate::paintRow( int x, int y, int width, unsigned char row,
                                     Uint32 forecolor, Uint32 backcolor )
{
  for ( int i = 0; i < width; i++ ) {
    const int offset = ( i * 8 ) / width;
    const int pixel = 1 - ( ( row >> offset ) & 1 );
    putPixel( x + i, y, ( pixel ? forecolor : backcolor ) );
  }
}

static int widthAtCell( int x, int w )
{
  const int base = w / 80;
  const int extra = w % 80;
  if ( x < extra ) return base + 1;
  return base;
}

static int heightAtCell( int y, int h )
{
  const int base = h / 25;
  const int extra = h % 25;
  if ( y < extra ) return base + 1;
  return base;
}

void SimplePainterPrivate::calculateCells()
{
  if (!surface) return;

  const int width = surface->w;
  const int height = surface->h;
  int cy = 0;
  for ( int y = 0; y < 25; y++ )
  {
    const int ch = fitSurface ? heightAtCell( y, height ) : 16;
    int cx = 0;
    for ( int x = 0; x < 80; x++ )
    {
      const int cw = fitSurface ? widthAtCell( x, width ) : 8;
      cells[y][x] = Cell( cx, cy, cw, ch );
      cx += cw;
    }
    cy += ch;
  }
}

void SimplePainterPrivate::paintChar( int x, int y, unsigned char c,
                                      unsigned char color, bool blinkOn )
{
  const bool blinking = (color >> 7) && blinkOn;
  Uint32 backcolor = colors[ (color>>4)&7 ];
  Uint32 forecolor = blinking ? backcolor : colors[ color&15 ];

  const Cell &cell = cells[y][x];

  const int width = cell.w;
  const int height = cell.h;

  // form block that the char will be drawn in.
  const int lx = cell.x;
  const int rx = lx + width - 1;
  const int uy = cell.y;
  const int dy = uy + height - 1;

  // make sure the block isn't entirely outside the window
  if ( lx >= surface->w || rx < 0 || uy >= surface->h || dy < 0 ) {
    return;
  }

  for ( int i = 0; i < height; i++ )
  {
    const int row = pixelRow( c, (i * 16) / height );
    paintRow( lx, uy + i, width, row, forecolor, backcolor );
  }
}

// ---------------------------------------------------------------------------

SimplePainter::SimplePainter()
  : ScreenPainter(),
    d( new SimplePainterPrivate() )
{
  for ( int y = 0; y < 25; y++ ) {
    for ( int x = 0; x < 80; x++ ) {
      d->cells[y][x] = Cell();
    }
  }
}

SimplePainter::~SimplePainter()
{
  delete d;
  d = 0;
}

void SimplePainter::setSDLSurface( SDL_Surface *surface )
{
  d->surface = surface;
  d->calculateCells();
}

void SimplePainter::begin_impl()
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

void SimplePainter::paintChar( int x, int y, unsigned char c, unsigned char color )
{
  d->cells[y][x].makeDirty( c, color, blinkOn() );
}

void SimplePainter::end_impl()
{
  if (!d->surface || !d->blitter) {
    zwarn() << "Attempted to paint without a begin called" << endl;
    return;
  }

  bool needFlip = false;

  for ( int y = 0; y < 25; y++ )
  {
    for ( int x = 0; x < 80; x++ )
    {
      Cell &cell = d->cells[y][x];

      if ( cell.isClean() ) {
        // no need to redraw this
        continue;
      }

      cell.makeClean();
      unsigned char color = cell.color;
      unsigned char c = cell.character;
      bool blink = cell.blinkOn;

      d->paintChar( x, y, c, color, blink );
      needFlip = true;
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

SDL_Surface * SimplePainter::createWindow( int w, int h, bool fullscreen )
{
  Uint32 surfaceFlags = SDL_RESIZABLE | SDL_SWSURFACE;
  if ( fullscreen ) {
    surfaceFlags |= SDL_FULLSCREEN;
  }

  SDL_Surface *display = SDL_SetVideoMode( w, h, 0, surfaceFlags );

  if (!display) {
    zerror() << "Could not create display:" << SDL_GetError();
  }
  else {
    SDL_FillRect( display, 0, 0 );
  }

  return display;
}

void SimplePainter::setFitToSurface( bool fit )
{
  d->fitSurface = fit;
  d->calculateCells();
}

