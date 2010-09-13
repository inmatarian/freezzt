/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef SIMPLE_PAINTER_H
#define SIMPLE_PAINTER_H

#include "abstractPainter.h"

class SimplePainterPrivate;

/// Simple 640x480 SDL surface painter
class SimplePainter : public AbstractPainter
{
  public:
    SimplePainter();
    virtual ~SimplePainter();

    /// sets the surface that will be painted on
    void setSDLSurface( SDL_Surface *surface );

    /// paintChar will draw a character and color at a given spot on the 80x25 grid
    virtual void paintChar( int x, int y, unsigned char c, unsigned char color );

    /// accessor
    static int screenWidth() { return 640; };

    /// accessor
    static int screenHeight() { return 400; };

    /// property
    virtual bool windowResizable() { return false; };

  protected:
    /// begin will be called before any painting is started
    virtual void begin_impl();

    /// end is called when all painting is done
    virtual void end_impl();

    /// access needed to a millisecond clock for blinking. think SDL_GetTicks
    virtual int currentTime();

  private:
    SimplePainterPrivate *d;
};

#endif // SIMPLE_PAINTER_H


