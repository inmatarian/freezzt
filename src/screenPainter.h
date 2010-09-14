/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef SCREEN_PAINTER_H
#define SCREEN_PAINTER_H

#include "abstractPainter.h"

struct SDL_Surface;

class ScreenPainter : public AbstractPainter
{
  public:
    /// sets the surface that will be painted on
    virtual void setSDLSurface( SDL_Surface *surface ) = 0;

    virtual void paintChar( int x, int y, unsigned char c, unsigned char color ) = 0;

    /// accessor
    static int screenWidth() { return 640; };

    /// accessor
    static int screenHeight() { return 400; };

    /// Handle making windows
    virtual SDL_Surface *createWindow( int x, int y, bool fullscreen ) = 0;

  protected:
    virtual int currentTime();

  private:
    bool m_blinkOn;
};

#endif // SCREEN_PAINTER_H

