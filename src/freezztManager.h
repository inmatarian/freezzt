/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef FREEZZT_MANAGER_H
#define FREEZZT_MANAGER_H

class AbstractPainter;
class AbstractEventLoop;

class FreeZZTManagerPrivate;

/// Main application class
class FreeZZTManager
{
  public:
    FreeZZTManager();
    virtual ~FreeZZTManager();

    /// provide arguments before executing
    void parseArgs( int argc, char ** argv );

    /// event loop sends events to manager
    void doKeypress( int keycode, int unicode );

    /// event loop triggers frame update
    void doFrame();

    /// sets the framerate while playing
    void setFrameRate( int hertz );
    /// accessor
    int frameRate() const;

    /// Requires a Painter before calling exec
    void setPainter( AbstractPainter *painter );

    /// Requires an Event Loop before calling exec
    void setEventLoop( AbstractEventLoop *eventLoop );

    /// executes game state machine
    virtual void exec();

  private:
    FreeZZTManagerPrivate *d;
};

#endif // FREEZZT_MANAGER_H

