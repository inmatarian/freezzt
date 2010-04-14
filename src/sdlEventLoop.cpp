/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <cassert>
#include <SDL.h>

#include "defines.h"
#include "freezztManager.h"
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

enum {
  USERCODE_FRAMEUPDATE = 1
};

// belongs to other thread, don't touch.
static Uint32 update_timer_callback(Uint32 interval, void *param)
{
  SDL_UserEvent userevent;
  userevent.type = SDL_USEREVENT;
  userevent.code = USERCODE_FRAMEUPDATE;
  userevent.data1 = 0;
  userevent.data2 = 0;

  SDL_Event event;
  event.type = SDL_USEREVENT;
  event.user = userevent;

  SDL_PushEvent(&event);
  return interval;
}

// ---------------------------------------------------------------------------

class SDLEventLoopPrivate
{
  public:
    SDLEventLoopPrivate( SDLEventLoop *pSelf )
      : pManager( 0 ),
        pPainter( 0 ),
        stop( false ),
        doFrame( false ),
        hasUpdateTimer( false ),
        updateTimerID( 0 ),
        self( pSelf )
    { /* */ };

    void parseEvent( const SDL_Event &event );

  public:
    FreeZZTManager *pManager;
    AbstractPainter *pPainter;
    bool stop;
    bool doFrame;
    bool hasUpdateTimer;
    SDL_TimerID updateTimerID;

  private:
    SDLEventLoop *self;
};

void SDLEventLoopPrivate::parseEvent( const SDL_Event &event )
{
  FreeZZTManager *zzt = self->manager();

  switch ( event.type )
  {
    case SDL_QUIT:
      stop = true;
      break;

    case SDL_KEYDOWN: {
      int keycode, unicode;
      translateSDLKeyToZZT( event.key.keysym, keycode, unicode );
      if ( keycode == Defines::Z_F10 ) {
        // during development, F10 is the auto-quit key
        stop = true;
      }
      zzt->doKeypress( keycode, unicode );
      break;
    }

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
  if (d->hasUpdateTimer) {
    SDL_RemoveTimer( d->updateTimerID );
  }

  delete d;
  d = 0;
}

void SDLEventLoop::exec()
{
  FreeZZTManager *zzt = manager();
  assert( zzt );
  zzt->begin();

  SDL_Event event;
  int lastClockUpdate = 0;
  while ( !d->stop )
  {
    SDL_WaitEvent( &event );
    d->parseEvent( event );
    if (d->stop) break;

    while ( SDL_PollEvent( &event ) ) {
      d->parseEvent( event );
      if (d->stop) break;
    }

    if (d->doFrame) {
      zzt->doUpdate();
      zzt->doPaint( painter() );
      d->doFrame = false;
    }
  }

  zzt->end();
}

void SDLEventLoop::setManager( FreeZZTManager *manager )
{
  d->pManager = manager;
}

FreeZZTManager *SDLEventLoop::manager() const
{
  return d->pManager;
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
  if (d->hasUpdateTimer) {
    SDL_RemoveTimer( d->updateTimerID );
  }

  d->hasUpdateTimer = true;
  d->updateTimerID = SDL_AddTimer(milliseconds, &update_timer_callback, 0);
}

