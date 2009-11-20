// Insert copyright and license information here.

#ifndef ABSTRACT_PAINTER_H
#define ABSTRACT_PAINTER_H

class AbstractPainter
{
  public:
    virtual void begin() = 0;
    virtual void paintChar( int x, int y, unsigned char c, unsigned char color ) = 0;
    virtual void end() = 0;

    virtual void drawText( int x, int y, unsigned char color, const std::string &text );
};

#endif // ABSTRACT_PAINTER_H
