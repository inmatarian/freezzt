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

    /// executes game state machine
    virtual void exec();

    virtual void setFrameRate( int hertz );
    virtual int frameRate() const;

  private:
    FreeZZTManagerPrivate *d;
};

#endif // FREEZZT_MANAGER_H

