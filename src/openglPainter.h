#ifndef OPENGL_PAINTER_H
#define OPENGL_PAINTER_H

#include "abstractPainter.h"
#include "screenPainter.h"

class OpenGLPainterPrivate;

/// Simple 640x480 SDL surface painter
class OpenGLPainter : public ScreenPainter
{
  public:
    OpenGLPainter();
    virtual ~OpenGLPainter();

    /// sets the surface that will be painted on
    virtual void setSDLSurface( SDL_Surface *surface );

    /// paintChar will draw a character and color at a given spot on the 80x25 grid
    virtual void paintChar( int x, int y, unsigned char c, unsigned char color );

    /// Handle making windows
    virtual SDL_Surface *createWindow( int w, int h, bool fullscreen );

  protected:
    /// begin will be called before any painting is started
    virtual void begin_impl();

    /// end is called when all painting is done
    virtual void end_impl();

  private:
    OpenGLPainterPrivate *d;
};

#endif

