/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef SDL_EVENT_LOOP_H
#define SDL_EVENT_LOOP_H

class SDLManager;
class FreeZZTManager;
class AbstractPainter;

class SDLEventLoopPrivate;
class SDLEventLoop
{
  public:
    SDLEventLoop();
    virtual ~SDLEventLoop();

    void exec();

    void setFrameLatency( int milliseconds );

    void setZZTManager( FreeZZTManager *manager );
    FreeZZTManager *zztManager() const;

    void setSDLManager( SDLManager *manager );
    SDLManager *sdlManager() const;

    void setPainter( AbstractPainter *painter );
    AbstractPainter *painter() const;

  private:
    SDLEventLoopPrivate *d;
};

#endif // SDL_EVENT_LOOP_H

