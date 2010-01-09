/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef ABSTRACT_PAINTER_H
#define ABSTRACT_PAINTER_H

/// Painting interface to abstract away screen drawing details
class AbstractPainter
{
  public:
    AbstractPainter()
      : m_blinkClock(0)
    {/* */};

    /// begin will be called before any painting is started
    void begin();

    /// end is called when all painting is done
    void end();

    /// paintChar will draw a character and color at a given spot on the 80x25 grid
    virtual void paintChar( int x, int y, unsigned char c, unsigned char color ) = 0;

    /// convienience function for writing text in a color at a given spot on the 80x25 grid
    virtual void drawText( int x, int y, unsigned char color, const std::string &text );

    /// list of colors from 0 to 15
    enum ColorChart {
      BLACK = 0,
      DARK_BLUE,
      DARK_GREEN,
      DARK_CYAN,
      DARK_RED,
      DARK_PURPLE,
      DARK_BROWN,
      GRAY,
      DARK_GRAY,
      BLUE,
      GREEN,
      CYAN,
      RED,
      MAGENTA,
      YELLOW,
      WHITE
    };

    /// blink cycle Draw Foregound when false, Draw background only when true
    bool blinkOn() const;

  protected:
    /// begin template method calls begin_impl
    virtual void begin_impl() {/* */};

    /// end template method calls end_impl
    virtual void end_impl() {/* */};

    /// access needed to a millisecond clock for blinking. think SDL_GetTicks
    virtual int currentTime() = 0;

  private:
    int m_blinkClock;
};

#endif // ABSTRACT_PAINTER_H
