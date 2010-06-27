/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <cassert>
#include <SDL.h>
#include <SDL_thread.h>

#include "defines.h"
#include "zstring.h"
#include "freezztManager.h"
#include "sdlManager.h"
#include "sdlEventLoop.h"

static void translateSDLKeyToZZT( const SDL_keysym &keysym,
                                  int &keycode,
                                  int &unicode )
{
  // most of the SDL unicodes are safe to use as straight zzt keys, though
  // we have a few that need filtering out.

  using namespace Defines;

  switch ( keysym.sym )
  {
    case SDLK_UP:
      keycode = ( keysym.mod & KMOD_SHIFT ) ? Z_ShootUp : Z_Up;
      unicode = 0;
      break;
    case SDLK_DOWN:
      keycode = ( keysym.mod & KMOD_SHIFT ) ? Z_ShootDown : Z_Down;
      unicode = 0;
      break;
    case SDLK_LEFT:
      keycode = ( keysym.mod & KMOD_SHIFT ) ? Z_ShootLeft : Z_Left;
      unicode = 0;
      break;
    case SDLK_RIGHT:
      keycode = ( keysym.mod & KMOD_SHIFT ) ? Z_ShootRight : Z_Right;
      unicode = 0;
      break;

    case SDLK_ESCAPE:    keycode = Z_Escape;    unicode = 0; break;
    case SDLK_RETURN:    keycode = Z_Enter;     unicode = 0; break;
    case SDLK_BACKSPACE: keycode = Z_Backspace; unicode = 0; break;
    case SDLK_TAB:       keycode = Z_Tab;       unicode = 0; break;
    case SDLK_HOME:      keycode = Z_Home;      unicode = 0; break;
    case SDLK_END:       keycode = Z_End;       unicode = 0; break;
    case SDLK_PAGEUP:    keycode = Z_PageUp;    unicode = 0; break;
    case SDLK_PAGEDOWN:  keycode = Z_PageDown;  unicode = 0; break;
    case SDLK_INSERT:    keycode = Z_Insert;    unicode = 0; break;
    case SDLK_DELETE:    keycode = Z_Delete;    unicode = 0; break;
    case SDLK_F1:        keycode = Z_F1;        unicode = 0; break;
    case SDLK_F2:        keycode = Z_F2;        unicode = 0; break;
    case SDLK_F3:        keycode = Z_F3;        unicode = 0; break;
    case SDLK_F4:        keycode = Z_F4;        unicode = 0; break;
    case SDLK_F5:        keycode = Z_F5;        unicode = 0; break;
    case SDLK_F6:        keycode = Z_F6;        unicode = 0; break;
    case SDLK_F7:        keycode = Z_F7;        unicode = 0; break;
    case SDLK_F8:        keycode = Z_F8;        unicode = 0; break;
    case SDLK_F9:        keycode = Z_F9;        unicode = 0; break;
    case SDLK_F10:       keycode = Z_F10;       unicode = 0; break;
    case SDLK_F11:       keycode = Z_F11;       unicode = 0; break;
    case SDLK_F12:       keycode = Z_F12;       unicode = 0; break;

    default:
      keycode = Z_Unicode;
      unicode = keysym.unicode;
      break;
  }
}

// ---------

enum {
  USERCODE_FRAMEUPDATE = 1,
  USERCODE_KEYUPDATE = 2
};

class MutexLocker
{
  public:
    MutexLocker( SDL_mutex *m ) : mutex(m) { if (mutex) SDL_mutexP(mutex); };
    ~MutexLocker() { if (mutex) SDL_mutexV(mutex); };
  private:
    SDL_mutex *mutex;
};

class UpdateThread
{
  public:
    // GUI THREAD CODE
    static void start();
    static void stop();

    static void setFrameLatency( Sint32 latency );

  protected:
    // UPDATE THREAD CODE
    static void lock();
    static void unlock();

    static Uint32 update_timer_callback(Uint32 interval, void *param);
    static void serviceFrameUpdate( Uint32 interval );

  private:
    static bool m_started;
    static SDL_TimerID m_id;
    static SDL_mutex *m_mutex;
    static Sint32 m_latency;
    static Sint32 m_latencyClock;
};

bool UpdateThread::m_started = false;
SDL_TimerID UpdateThread::m_id = 0;
SDL_mutex *UpdateThread::m_mutex = 0;
Sint32 UpdateThread::m_latency = 27;
Sint32 UpdateThread::m_latencyClock = 0;

void UpdateThread::start()
{
  if ( m_started ) return;
  m_id = SDL_AddTimer(10, &update_timer_callback, 0);
  m_mutex = SDL_CreateMutex();
  m_started = true;
}

void UpdateThread::stop()
{
  if ( !m_started ) return;
  SDL_RemoveTimer( m_id );
  SDL_DestroyMutex( m_mutex );
  m_mutex = 0;
  m_started = false;
}

void UpdateThread::setFrameLatency( Sint32 latency )
{
  MutexLocker lock( m_mutex );
  m_latency = latency;
  m_latencyClock = 0;
}

void UpdateThread::serviceFrameUpdate( Uint32 interval )
{
  // Generate a Frame Update event when enough time has passed.
  // Drop lost update when too much time has passed.
  m_latencyClock += interval;

  if ( m_latencyClock < m_latency ) {
    return;
  }

  if ( m_latencyClock >= m_latency * 3 ) {
    m_latencyClock = 0;
  }
  else {
    m_latencyClock -= m_latency;
  }
 
  SDL_UserEvent userevent;
  userevent.type = SDL_USEREVENT;
  userevent.code = USERCODE_FRAMEUPDATE;
  userevent.data1 = 0;
  userevent.data2 = 0;

  SDL_Event event;
  event.type = SDL_USEREVENT;
  event.user = userevent;

  SDL_PushEvent(&event);
}

Uint32 UpdateThread::update_timer_callback(Uint32 interval, void *param)
{
  serviceFrameUpdate( interval );
  return interval;
}

// ---------------------------------------------------------------------------

class SDLEventLoopPrivate
{
  public:
    SDLEventLoopPrivate( SDLEventLoop *pSelf );
    ~SDLEventLoopPrivate();

    void parseEvent( const SDL_Event &event );

  public:
    FreeZZTManager *pZZTManager;
    SDLManager *pSDLManager;
    AbstractPainter *pPainter;
    bool stop;
    bool doFrame;

  private:
    SDLEventLoop *self;
};

SDLEventLoopPrivate::SDLEventLoopPrivate( SDLEventLoop *pSelf )
  : pZZTManager( 0 ),
    pSDLManager( 0 ),
    pPainter( 0 ),
    stop( false ),
    doFrame( false ),
    self( pSelf )
{
  /* */
}

SDLEventLoopPrivate::~SDLEventLoopPrivate()
{
  UpdateThread::stop();
}

void SDLEventLoopPrivate::parseEvent( const SDL_Event &event )
{
  switch ( event.type )
  {
    case SDL_QUIT:
      stop = true;
      break;

    case SDL_KEYDOWN: {
      int keycode, unicode;
      translateSDLKeyToZZT( event.key.keysym, keycode, unicode );
      switch ( keycode )
      {
        case Defines::Z_F10:
          // during development, F10 is the auto-quit key
          stop = true;
          break;
        case Defines::Z_F11:
          pSDLManager->toggleFullScreen();
          break;
        default:
          pZZTManager->doKeypress( keycode, unicode );
          break;
      }
      break;
    }

    case SDL_VIDEORESIZE:
      pSDLManager->doResize( event.resize.w, event.resize.h ); 
      break;

    case SDL_USEREVENT:
      if ( event.user.code == USERCODE_FRAMEUPDATE ) {
        doFrame = true;
      }
      break;

    default: break;
  }
}

SDLEventLoop::SDLEventLoop()
  : d( new SDLEventLoopPrivate(this) )
{
  /* */
}

SDLEventLoop::~SDLEventLoop()
{
  delete d;
  d = 0;
}

void SDLEventLoop::exec()
{
  assert( d->pZZTManager );
  d->pZZTManager->begin();

  UpdateThread::start();

  SDL_Event event;
  int lastClockUpdate = 0;
  while ( !d->stop && !d->pZZTManager->quitting() )
  {
    SDL_WaitEvent( &event );
    d->parseEvent( event );
    if (d->stop) break;

    while ( SDL_PollEvent( &event ) ) {
      d->parseEvent( event );
      if (d->stop) break;
    }

    if (d->doFrame) {
      d->pZZTManager->doUpdate();
      d->pZZTManager->doPaint( painter() );
      d->doFrame = false;
    }
  }

  UpdateThread::stop();

  d->pZZTManager->end();
}

void SDLEventLoop::setZZTManager( FreeZZTManager *manager )
{
  d->pZZTManager = manager;
}

FreeZZTManager *SDLEventLoop::zztManager() const
{
  return d->pZZTManager;
}

void SDLEventLoop::setSDLManager( SDLManager *manager )
{
  d->pSDLManager = manager;
}

SDLManager *SDLEventLoop::sdlManager() const
{
  return d->pSDLManager;
}

void SDLEventLoop::setPainter( AbstractPainter *painter )
{
  d->pPainter = painter;
}

AbstractPainter *SDLEventLoop::painter() const
{
  return d->pPainter;
}

void SDLEventLoop::setFrameLatency( int milliseconds )
{
  UpdateThread::setFrameLatency( milliseconds );
}

