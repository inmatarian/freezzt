/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef FREEZZT_MANAGER_H
#define FREEZZT_MANAGER_H

class AbstractPainter;
class AbstractMusicStream;
class AbstractFileModelFactory;
class GameWorld;
class FreeZZTManagerPrivate;

/// Main application class
class FreeZZTManager
{
  public:
    FreeZZTManager();
    virtual ~FreeZZTManager();

    /// provide before executing
    void loadWorld( const char *filename );

    /// currently set world, if any
    GameWorld *world() const;

    /// set world if you want
    void setWorld( GameWorld *world );

    /// Set factory
    void setFileModelFactory( AbstractFileModelFactory *factory );

    /// Set factory
    void setMusicStream( AbstractMusicStream *stream );

    /// set speed visible on title screen, 0 to 8
    void setSpeed( int value );

    /// begins the game state machine, call before any of the do functions.
    void begin();

    /// event loop sends events to manager
    void doKeypress( int keycode, int unicode );

    /// event loop triggers frame update
    void doUpdate();

    /// event loop triggers frame redraw
    void doPaint( AbstractPainter *painter );

    /// ends the game state machine, call on shutdown.
    void end();

    /// signals state machine shutdown from within.
    bool quitting() const;

  private:
    FreeZZTManagerPrivate *d;
};

#endif // FREEZZT_MANAGER_H

