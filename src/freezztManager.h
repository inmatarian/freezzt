/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef FREEZZT_MANAGER_H
#define FREEZZT_MANAGER_H

#include "abstractManager.h"

class FreeZZTManagerPrivate;

/// Main application class
class FreeZZTManager : public AbstractManager
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

    /// executes game state machine
    virtual void exec();

    /// sets the framerate while playing
    virtual void setFrameRate( int hertz );
    /// accessor
    virtual int frameRate() const;

  private:
    FreeZZTManagerPrivate *d;
};

#endif // FREEZZT_MANAGER_H

