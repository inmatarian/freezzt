/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef SIMPLE_PAINTER_H
#define SIMPLE_PAINTER_H

#include "abstractPainter.h"
#include "screenPainter.h"

class SimplePainterPrivate;

/// Simple 640x480 SDL surface painter
class SimplePainter : public ScreenPainter
{
  public:
    SimplePainter();
    virtual ~SimplePainter();

    /// sets the surface that will be painted on
    virtual void setSDLSurface( SDL_Surface *surface );

    /// paintChar will draw a character and color at a given spot on the 80x25 grid
    virtual void paintChar( int x, int y, unsigned char c, unsigned char color );

    /// property
    void setFitToSurface( bool fit );

    /// Handle making windows
    virtual SDL_Surface *createWindow( int w, int h, bool fullscreen );

  protected:
    /// begin will be called before any painting is started
    virtual void begin_impl();

    /// end is called when all painting is done
    virtual void end_impl();

  private:
    SimplePainterPrivate *d;
};

#endif // SIMPLE_PAINTER_H


