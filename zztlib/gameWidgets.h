/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef __GAME_WIDGETS_H__
#define __GAME_WIDGETS_H__

class AbstractPainter;
class GameWorld;

class GameWidget
{
  public:
    GameWidget() : m_row(0), m_column(0), m_focused(false) {/* */};
    virtual ~GameWidget() { /* */ };

    void setRow( int row ) { m_row = row; };
    int row() const { return m_row; };

    void setColumn( int column ) { m_column = column; };
    int column() const { return m_column; };

    void setFocused( bool f = true ) { m_focused = f; };
    bool focused() const { return m_focused; };

    void setCentered( int offset, int width, int max_width )
    {
      m_column = offset + ( ( max_width - width ) / 2 );
    };

    virtual void doKeypress( int keycode, int unicode ) { /* */ };
    virtual void doPaint( AbstractPainter *painter ) { /* */ };

  protected:
    static void drawButtonLine( AbstractPainter *painter,
                                int column, int row,
                                const std::string &buttxt, int butcolor,
                                const std::string &valtxt, int txtcolor );

    static void drawCenteredTextLine( AbstractPainter *painter, int column,
                                      int row, const std::string &txt,
                                      int txtcolor, int spacecolor );

    static void drawItemLine( AbstractPainter *painter,
                              int column, int row,
                              int itempic, int itemcolor,
                              const std::string &txt, int txtcolor,
                              int itemval );

    static void drawKeysLine( AbstractPainter *painter,
                              int column, int row,
                              int itempic, int itemcolor,
                              const std::string &txt, int txtcolor,
                              GameWorld *world );

  private:
    int m_row;
    int m_column;
    bool m_focused;
};

// -----------------------------------------------------------------

class FramerateSliderWidget : public GameWidget
{
  public:
    FramerateSliderWidget();
    virtual void doKeypress( int keycode, int unicode );
    virtual void doPaint( AbstractPainter *painter );

    void setValue( int value ) { m_value = value; };
    int value() const { return m_value; };
    const char *str() const;

  private:
    int m_value;
};

// -----------------------------------------------------------------

class TextInputWidget : public GameWidget
{
  public:
    TextInputWidget();

    void setFixedSuffix( const std::string &sufx ) { /* */ };

    virtual void doKeypress( int keycode, int unicode );
    virtual void doPaint( AbstractPainter *painter );

    std::string value() const { return userMessage + fixedSuffix; };
    std::string str() const;

    void reset() { userMessage.erase(); fixedSuffix.erase(); };

  private:
    std::string userMessage;
    std::string fixedSuffix;
};

// -----------------------------------------------------------------

class GameWorldAwareWidget: public GameWidget
{
  public:
    GameWorldAwareWidget() : m_world(0) { /* */ };

    void setWorld( GameWorld *world ) { m_world = world; };
    GameWorld *world() const { return m_world; };

  private:
    GameWorld *m_world;
};

// -----------------------------------------------------------------

class TitleModeInfoBarWidget : public GameWidget
{
  public:
    TitleModeInfoBarWidget();
    virtual void doPaint( AbstractPainter *painter );

  public:
    FramerateSliderWidget framerateSliderWidget;
};

// -----------------------------------------------------------------

class PlayModeInfoBarWidget : public GameWorldAwareWidget
{
  public:
    PlayModeInfoBarWidget();
    virtual void doPaint( AbstractPainter *painter );
};

#endif /* __GAME_WIDGETS_H__ */

