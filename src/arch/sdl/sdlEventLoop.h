/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef SDL_EVENT_LOOP_H
#define SDL_EVENT_LOOP_H

#include "abstractEventLoop.h"

class SDLEventLoopPrivate;
class SDLEventLoop : public AbstractEventLoop
{
  public:
    SDLEventLoop();
    virtual ~SDLEventLoop();

    virtual void exec();
    virtual void sleep( int milliseconds );
    virtual int clock() const;
    virtual void stop();
    virtual void setFrameLatency( int milliseconds );

  private:
    SDLEventLoopPrivate *d;
};

#endif // SDL_EVENT_LOOP_H

