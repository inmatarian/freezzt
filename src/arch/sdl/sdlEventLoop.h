/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef SDL_EVENT_LOOP_H
#define SDL_EVENT_LOOP_H

#include "abstractEventLoop.h"

class SDLEventLoop : public AbstractEventLoop
{
  public:
    SDLEventLoop()
      : AbstractEventLoop(),
        m_stop( false )
    {/* */};

    virtual void exec();
    virtual void sleep( int untilTime );
    virtual int clock() const;
    virtual void stop() { m_stop = true; };

  private:
    bool m_stop;
};

#endif // SDL_EVENT_LOOP_H

