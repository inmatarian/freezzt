/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef FREEZZT_MANAGER_H
#define FREEZZT_MANAGER_H

class AbstractPlatformServices;
class AbstractPainter;
class FreeZZTManagerPrivate;

/// Main application class
class FreeZZTManager
{
  public:
    FreeZZTManager();
    virtual ~FreeZZTManager();

    /// provide before executing
    void loadWorld( const char *filename );

    /// Set services
    void setServices( AbstractPlatformServices *services );

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

  private:
    FreeZZTManagerPrivate *d;
};

#endif // FREEZZT_MANAGER_H

