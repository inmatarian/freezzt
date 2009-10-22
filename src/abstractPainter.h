// Insert copyright and license information here.

#ifndef ABSTRACT_PAINTER_H
#define ABSTRACT_PAINTER_H

struct SDL_Surface;

class AbstractPainter
{
  public:
    virtual void begin() = 0;
    virtual void paintChar( int x, int y, unsigned char c, unsigned char color ) = 0;
    virtual void end() = 0;
};

#endif // ABSTRACT_PAINTER_H
