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
#include "openglPainter.h"

using namespace std;

// -----------------------------------------------------------------

class OpenGLPainterPrivate
{
  public:
    OpenGLPainterPrivate();
    ~OpenGLPainterPrivate();

    void createGLState();
    void render();

  public:
    SimplePainter simplePainter;
    SDL_Surface *simpleSurface;
    SDL_Surface *glSurface;

    GLuint texture;
    GLuint textureFormat;
    float polyWidth;
    float polyHeight;
};

static const int TEXTURE_WIDTH = 1024;
static const int TEXTURE_HEIGHT = 512;

OpenGLPainterPrivate::OpenGLPainterPrivate()
{
  simpleSurface = SDL_CreateRGBSurface( SDL_SWSURFACE,
      TEXTURE_WIDTH, TEXTURE_HEIGHT, 32, 0, 0, 0, 0 );
  simplePainter.setFitToSurface( false );
  simplePainter.setSDLSurface( simpleSurface );
}

OpenGLPainterPrivate::~OpenGLPainterPrivate()
{
  SDL_FreeSurface( simpleSurface );
  simpleSurface = 0;
  glDeleteTextures( 1, &texture );
}

static void checkGLError()
{
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    zdebug() << err;
  }
}

void OpenGLPainterPrivate::createGLState()
{
  float width = glSurface->w;
  float height = glSurface->h;
  glEnable( GL_TEXTURE_2D );
  glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
  glViewport( 0, 0, width, height );
  glClear( GL_COLOR_BUFFER_BIT );
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glOrtho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  glGenTextures( 1, &texture );
	glBindTexture( GL_TEXTURE_2D, texture );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  textureFormat = (simpleSurface->format->Rmask == 0x0000ff)
    ? GL_RGBA
    : GL_BGRA;

  polyWidth = float(TEXTURE_WIDTH) * float(glSurface->w) / 640.0f;
  polyHeight = float(TEXTURE_HEIGHT) * float(glSurface->h) / 400.0f;
}

void OpenGLPainterPrivate::render()
{
	glTexImage2D( GL_TEXTURE_2D, 0, 4, TEXTURE_WIDTH, TEXTURE_HEIGHT,
                0, textureFormat, GL_UNSIGNED_BYTE, simpleSurface->pixels );

  glBindTexture( GL_TEXTURE_2D, texture );
   
  glBegin( GL_QUADS );
    //Bottom-left vertex (corner)
    glTexCoord2i( 0, 0 );
    glVertex3f( 0.0f, 0.0f, 0.0f );
    
    //Bottom-right vertex (corner)
    glTexCoord2i( 1, 0 );
    glVertex3f( polyWidth, 0.0f, 0.0f );
    
    //Top-right vertex (corner)
    glTexCoord2i( 1, 1 );
    glVertex3f( polyWidth, polyHeight, 0.0f );
    
    //Top-left vertex (corner)
    glTexCoord2i( 0, 1 );
    glVertex3f( 0.f, polyHeight, 0.0f );
  glEnd();
  checkGLError();

  SDL_GL_SwapBuffers();
}

// -----------------------------------------------------------------

OpenGLPainter::OpenGLPainter()
  : d( new OpenGLPainterPrivate )
{
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
}

OpenGLPainter::~OpenGLPainter()
{
  delete d;
}

void OpenGLPainter::setSDLSurface( SDL_Surface *surface )
{
  d->glSurface = surface;
  d->createGLState();
}

void OpenGLPainter::paintChar( int x, int y, unsigned char c, unsigned char color )
{
  d->simplePainter.paintChar( x, y, c, color );
}

void OpenGLPainter::begin_impl()
{
  d->simplePainter.begin();
}

void OpenGLPainter::end_impl()
{
  d->simplePainter.end();
  d->render();
}

SDL_Surface * OpenGLPainter::createWindow( int w, int h, bool fullscreen )
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

