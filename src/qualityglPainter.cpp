/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <iostream>
#include <SDL.h>
#include <SDL_opengl.h>

#include "debug.h"
#include "zstring.h"
#include "abstractPainter.h"
#include "simplePainter.h"
#include "qualityglPainter.h"

#include "page437_8x16.xbm"

using namespace std;

// -----------------------------------------------------------------

struct ZZTGLCell
{
  unsigned char character;
  unsigned char color;
  bool blink;

  ZZTGLCell( unsigned char pchar = 0, unsigned char pcolor = 0, bool b=false )
    : character(pchar), color(pcolor), blink(b) { /* */ }; 
};

struct ZZTGLColor
{
  GLubyte red;
  GLubyte green;
  GLubyte blue;

  ZZTGLColor( GLbyte r = 0, GLbyte g = 0, GLbyte b = 0 )
    : red(r), green(g), blue(b) { /* */ };
};

// -----------------------------------------------------------------

static const int MAX_TEXTURES = 256;

class QualityGLPainterPrivate
{
  public:
    QualityGLPainterPrivate();
    ~QualityGLPainterPrivate();

    static unsigned char pixelRow( unsigned char c, int row );
    static void renderCharacter( SDL_Surface *surface, int id, Uint32 white );

    void createGLState();
    void drawGLSquare();
    void drawTexturedGLSquare();
    void renderScene();

  public:
    SDL_Surface *glSurface;
    ZZTGLCell cells[25][80];
    ZZTGLColor colors[16];

    GLuint textures[MAX_TEXTURES];
    GLuint displayListBackground;
    GLuint displayListTextured;
};

QualityGLPainterPrivate::QualityGLPainterPrivate()
  : glSurface( 0 )
{
  /* */
}

QualityGLPainterPrivate::~QualityGLPainterPrivate()
{
  glDeleteTextures( MAX_TEXTURES, textures );
}

unsigned char QualityGLPainterPrivate::pixelRow( unsigned char c, int row )
{
  const int pitch = page437_8x16_width / 8;
  const int volume = page437_8x16_height / 16 * pitch;
  const int target = ( row * pitch ) + ( c / 16 * volume ) + ( c % 16 );
  return page437_8x16_bits[target];
}

void QualityGLPainterPrivate::renderCharacter( SDL_Surface *surface, int id, Uint32 white )
{
  SDL_FillRect( surface, 0, surface->format->colorkey );
  void *pixels = surface->pixels;
  const int bpp = surface->format->BytesPerPixel;
  const int pitch = surface->pitch;

  for ( int y = 0; y < 16; y++ )
  {
    const int row = pixelRow( id, y );
    for ( int x = 0; x < 8; x++ )
    {
      const int pel = 1 - ( ( row >> x ) & 1 );
      if (!pel) continue;
      Uint8 *p = (Uint8 *)pixels + (y * pitch) + (x * bpp);
      *(Uint32 *)p = white;
    }
  }
}

void QualityGLPainterPrivate::createGLState()
{
  float width = glSurface->w;
  float height = glSurface->h;

  glEnable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable( GL_COLOR_MATERIAL );
  glViewport( 0, 0, width, height );
  glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
  glClear( GL_COLOR_BUFFER_BIT );
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glOrtho(0.0f, 80.0f, 25.0f, 0.0f, -1.0f, 1.0f);
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  SDL_Surface *textureSurface = SDL_CreateRGBSurface( SDL_SWSURFACE,
      8, 16, 32, 0, 0, 0, 0 );
  Uint32 blackColor = SDL_MapRGBA( textureSurface->format, 0x00, 0x00, 0x00,
      SDL_ALPHA_TRANSPARENT );
  Uint32 whiteColor = SDL_MapRGBA( textureSurface->format, 0xff, 0xff, 0xff,
      SDL_ALPHA_OPAQUE );
  SDL_SetColorKey( textureSurface, SDL_SRCCOLORKEY, blackColor );

  glGenTextures( MAX_TEXTURES, textures );
  for ( int i = 0; i <= MAX_TEXTURES; i++ )
  {
	  glBindTexture( GL_TEXTURE_2D, textures[i] );

    renderCharacter( textureSurface, i, whiteColor );
    SDL_Surface* displaySurface = SDL_DisplayFormatAlpha(textureSurface);

    GLuint textureFormat = (displaySurface->format->Rmask == 0x0000ff)
      ? GL_RGBA
      : GL_BGRA;

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

    glTexImage2D( GL_TEXTURE_2D, 0, 4, 8, 16, 0,
                  textureFormat, GL_UNSIGNED_BYTE, displaySurface->pixels );

    SDL_FreeSurface( displaySurface );
  }

  colors[0] = ZZTGLColor( 0x00, 0x00, 0x00 );
  colors[1] = ZZTGLColor( 0x00, 0x00, 0xaa );
  colors[2] = ZZTGLColor( 0x00, 0xaa, 0x00 );
  colors[3] = ZZTGLColor( 0x00, 0xaa, 0xaa );
  colors[4] = ZZTGLColor( 0xaa, 0x00, 0x00 );
  colors[5] = ZZTGLColor( 0xaa, 0x00, 0xaa );
  colors[6] = ZZTGLColor( 0xaa, 0x55, 0x00 );
  colors[7] = ZZTGLColor( 0xaa, 0xaa, 0xaa );
  colors[8] = ZZTGLColor( 0x55, 0x55, 0x55 );
  colors[9] = ZZTGLColor( 0x55, 0x55, 0xff );
  colors[10] = ZZTGLColor( 0x55, 0xff, 0x55 );
  colors[11] = ZZTGLColor( 0x55, 0xff, 0xff );
  colors[12] = ZZTGLColor( 0xff, 0x55, 0x55 );
  colors[13] = ZZTGLColor( 0xff, 0x55, 0xff );
  colors[14] = ZZTGLColor( 0xff, 0xff, 0x55 );
  colors[15] = ZZTGLColor( 0xff, 0xff, 0xff );

  SDL_FreeSurface( textureSurface );

  displayListBackground = glGenLists(1);
  glNewList(displayListBackground, GL_COMPILE);
    drawGLSquare();
  glEndList();

  displayListTextured = glGenLists(1);
  glNewList(displayListTextured, GL_COMPILE);
    drawTexturedGLSquare();
  glEndList();
}

void QualityGLPainterPrivate::drawGLSquare()
{
  glBegin( GL_QUADS );
    glVertex3f( 0.0f, 0.0f, 0.0f );
    glVertex3f( 1.0f, 0.0f, 0.0f );
    glVertex3f( 1.0f, 1.0f, 0.0f );
    glVertex3f( 0.0f, 1.0f, 0.0f );
  glEnd();
}

void QualityGLPainterPrivate::drawTexturedGLSquare()
{
  glBegin( GL_QUADS );
    glTexCoord2i( 0, 0 );
    glVertex3f( 0.0f, 0.0f, 0.0f );
    glTexCoord2i( 1, 0 );
    glVertex3f( 1.0f, 0.0f, 0.0f );
    glTexCoord2i( 1, 1 );
    glVertex3f( 1.0f, 1.0f, 0.0f );
    glTexCoord2i( 0, 1 );
    glVertex3f( 0.0f, 1.0f, 0.0f );
  glEnd();
}

void QualityGLPainterPrivate::renderScene()
{
  glClear( GL_COLOR_BUFFER_BIT );
  for ( int y = 0; y < 25; y++ )
  {
    for ( int x = 0; x < 80; x++ )
    {
      const ZZTGLCell &c = cells[y][x];
      const ZZTGLColor &fore = colors[ c.color & 0x0F ];
      const ZZTGLColor &back = colors[ ( c.color & 0x70 ) >> 4 ];
      const bool blink = c.blink && (c.color & 0x80);

      glPushMatrix();
      glTranslatef( x, y, 0.0f );

      glColor3ub( back.red, back.green, back.blue );
      glBindTexture( GL_TEXTURE_2D, textures[219] );
      glCallList(displayListTextured);
      // glCallList(displayListBackground);

      if ( !blink ) {
        glColor3ub( fore.red, fore.green, fore.blue );
        glBindTexture( GL_TEXTURE_2D, textures[c.character] );
        glCallList(displayListTextured);
      }

      glPopMatrix();
    }
  }
  SDL_GL_SwapBuffers();
}

// -----------------------------------------------------------------

QualityGLPainter::QualityGLPainter()
  : ScreenPainter(),
    d( new QualityGLPainterPrivate() )
{
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
}

QualityGLPainter::~QualityGLPainter()
{
  delete d;
}

void QualityGLPainter::setSDLSurface( SDL_Surface *surface )
{
  d->glSurface = surface;
  d->createGLState();
}

void QualityGLPainter::paintChar( int x, int y, unsigned char c, unsigned char color )
{
  d->cells[y][x] = ZZTGLCell( c, color, blinkOn() );
}

SDL_Surface *QualityGLPainter::createWindow( int w, int h, bool fullscreen )
{
  Uint32 surfaceFlags = SDL_RESIZABLE | SDL_OPENGL;
  if ( fullscreen ) {
    surfaceFlags |= SDL_FULLSCREEN;
  }

  SDL_Surface *display = SDL_SetVideoMode( w, h, 0, surfaceFlags );

  if (!display) {
    zerror() << "Could not create display:" << SDL_GetError();
  }

  return display;
}

void QualityGLPainter::begin_impl()
{
  /* lol, what */
}

void QualityGLPainter::end_impl()
{
  d->renderScene();
}

