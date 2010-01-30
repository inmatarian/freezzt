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
#include "abstractEventLoop.h"
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

void SDLEventLoop::exec()
{
  FreeZZTManager *zzt = manager();
  assert( zzt );

  while ( !m_stop )
  {
    SDL_Event event;
    while ( SDL_PollEvent( &event ) )
    {
      switch ( event.type )
      {
        case SDL_QUIT: {
          return;
        }

        case SDL_KEYDOWN: {
          int keycode, unicode;
          translateSDLKeyToZZT( event.key.keysym, keycode, unicode );
          zzt->doKeypress( keycode, unicode );
          break;
        }

        default: break;
      }
    }

    zzt->doFrame();
  }
}

int SDLEventLoop::clock() const
{
  return SDL_GetTicks();
};

void SDLEventLoop::sleep( int milliseconds )
{
  SDL_Delay( milliseconds );
}

