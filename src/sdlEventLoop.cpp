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

// ---------------------------------------------------------------------------

enum {
  USERCODE_NO_EVENT = 0,
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
    UpdateThread();
    ~UpdateThread() { stop(); };

    void start();
    void stop();
    bool started() const { return m_started; };

    void setLatency( Uint32 latency );
    void setEvent( int event );

  protected:
    UpdateThread( const UpdateThread &other ) { /* No Copying */ };

    // UPDATE THREAD CODE
    static Uint32 update_timer_callback(Uint32 interval, void *param);
    void serviceUpdate();

  private:
    bool m_started;
    SDL_TimerID m_id;
    Uint32 m_latency;
    Uint32 m_lastClock;
    int m_event;
};

UpdateThread::UpdateThread()
  : m_started( false ),
    m_id( 0 ),
    m_latency( 27 ),
    m_lastClock( 0 ),
    m_event( USERCODE_NO_EVENT )
{
  /* */
}

void UpdateThread::start()
{
  if ( m_started ) return;
  m_id = SDL_AddTimer(10, &update_timer_callback, this);
  m_started = true;
}

void UpdateThread::stop()
{
  if ( !m_started ) return;
  SDL_RemoveTimer( m_id );
  m_started = false;
}

void UpdateThread::setLatency( Uint32 latency )
{
  if ( m_started ) return; // Can't update while running.
  m_latency = latency;
}

void UpdateThread::setEvent( int event )
{
  if ( m_started ) return; // Can't update while running.
  m_event = event;
}

void UpdateThread::serviceUpdate()
{
  Uint32 clock = SDL_GetTicks();
  Uint32 interval;
  if (m_lastClock > clock) {
    m_lastClock = 0;
    interval = 0;
  }
  else {
    interval = clock - m_lastClock;
  }

  // Generate a Update event when enough time has passed.
  if ( interval < m_latency ) return;

  if ( interval >= m_latency * 5 ) {
    // Drop lost update when too much time has passed.
    m_lastClock = clock;
  }
  else {
    m_lastClock += m_latency;
  }
 
  SDL_UserEvent userevent;
  userevent.type = SDL_USEREVENT;
  userevent.code = m_event;
  userevent.data1 = 0;
  userevent.data2 = 0;

  SDL_Event event;
  event.type = SDL_USEREVENT;
  event.user = userevent;

  SDL_PushEvent(&event);
}

Uint32 UpdateThread::update_timer_callback(Uint32 interval, void *param)
{
  UpdateThread *thread = static_cast<UpdateThread *>(param);
  thread->serviceUpdate();
  return interval;
}

// ---------------------------------------------------------------------------

class JoystickHandler
{
  public:
    JoystickHandler();

    void handleJoyAxis( const SDL_Event &event );
    void handleJoyButton( const SDL_Event &event );
    void generateEvents( FreeZZTManager *pZZTManager );

  private:
    int joy_up;
    int joy_down;
    int joy_left;
    int joy_right;
    int joy_action;
};

JoystickHandler::JoystickHandler()
  : joy_up( 0 ),
    joy_down( 0 ),
    joy_left( 0 ),
    joy_right( 0 ),
    joy_action( 0 )
{
  /* */
}

void JoystickHandler::handleJoyAxis( const SDL_Event &event )
{
  switch ( event.jaxis.axis ) {
    case 0: {
      if ( event.jaxis.value < -4095 ) { // LEFT
        joy_left += 1;
      }
      else if ( event.jaxis.value > 4095 ) { // RIGHT
        joy_right += 1;
      }
      else {
        joy_left = 0;
        joy_right = 0;
      }
      break;
    }
    case 1: {
      if ( event.jaxis.value < -4095 ) { // UP
        joy_up += 1;
      }
      else if ( event.jaxis.value > 4095 ) { // DOWN
        joy_down += 1;
      }
      else {
        joy_up = 0;
        joy_down = 0;
      }
      break;
    }
    default: break;
  }

}

void JoystickHandler::handleJoyButton( const SDL_Event &event )
{
  switch ( event.jbutton.button ) {
    case 0:
      switch ( event.jbutton.state ) {
        case SDL_PRESSED:
          joy_action += 1;
          break;
        default:
          joy_action = 0;
          break;
      }
      break;
    default: break;
  }
}

static void testFireEvent( FreeZZTManager *pZZTManager, int &key, int action,
                           Defines::KeyCode move, Defines::KeyCode shoot )
{
  if (!key) return;
  
  if ( key == 1 || key >= 15 ) {
    pZZTManager->doKeypress( action ? shoot : move, 0 );
  }

  key += 1;
}

void JoystickHandler::generateEvents( FreeZZTManager *pZZTManager )
{
  using namespace Defines;
  testFireEvent( pZZTManager, joy_up, joy_action, Z_Up, Z_ShootUp );
  testFireEvent( pZZTManager, joy_down, joy_action, Z_Down, Z_ShootDown );
  testFireEvent( pZZTManager, joy_left, joy_action, Z_Left, Z_ShootLeft );
  testFireEvent( pZZTManager, joy_right, joy_action, Z_Right, Z_ShootRight );
}

// ---------------------------------------------------------------------------

class SDLEventLoopPrivate
{
  public:
    SDLEventLoopPrivate( SDLEventLoop *pSelf );
    void parseEvent( const SDL_Event &event );

  public:
    FreeZZTManager *pZZTManager;
    SDLManager *pSDLManager;
    AbstractPainter *pPainter;
    bool stop;
    bool doFrame;

    JoystickHandler joystickHandler;
    UpdateThread frameThread;

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
  frameThread.setEvent( USERCODE_FRAMEUPDATE );
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

    case SDL_JOYAXISMOTION: {
      joystickHandler.handleJoyAxis( event );
      break;
    }

    case SDL_JOYBUTTONDOWN: {
      joystickHandler.handleJoyButton( event );
      break;
    }

    case SDL_JOYBUTTONUP: {
      joystickHandler.handleJoyButton( event );
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

  d->frameThread.start();

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
      d->joystickHandler.generateEvents( d->pZZTManager );
      d->pZZTManager->doUpdate();
      d->pZZTManager->doPaint( painter() );
      d->doFrame = false;
    }
  }

  d->frameThread.stop();

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
  bool running = d->frameThread.started();
  if ( running ) d->frameThread.stop();

  d->frameThread.setLatency( milliseconds );

  if ( running ) d->frameThread.start();
}

