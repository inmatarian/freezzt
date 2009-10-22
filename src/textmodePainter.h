// Insert copyright and license information here.

#ifndef TEXTMODE_PAINTER_H
#define TEXTMODE_PAINTER_H

#include "abstractPainter.h"

class TextmodePainterPrivate;
class TextmodePainter : public AbstractPainter
{
  public:
    TextmodePainter();
    virtual ~TextmodePainter();

    void setSDLSurface( SDL_Surface *surface );

    virtual void begin();
    virtual void paintChar( int x, int y, unsigned char c, unsigned char color );
    virtual void end();

  private:
    TextmodePainterPrivate *d;
};

#endif // TEXTMODE_PAINTER_H

