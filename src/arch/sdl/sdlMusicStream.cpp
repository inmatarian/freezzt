/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include "debug.h"
#include "abstractMusicStream.h"
#include "sdlMusicStream.h"

class SDLMusicStreamPrivate
{
  public:
    SDLMusicStreamPrivate();
    ~SDLMusicStreamPrivate();
};

SDLMusicStreamPrivate::SDLMusicStreamPrivate()
{
  /* */
}

SDLMusicStreamPrivate::~SDLMusicStreamPrivate()
{
  /* */
}

SDLMusicStream::SDLMusicStream()
  : AbstractMusicStream(),
    d( new SDLMusicStreamPrivate )
{
  /* */
}

SDLMusicStream::~SDLMusicStream()
{
  delete d;
  d = 0;
}

void SDLMusicStream::playMusic( const char *song, PriorityType priority )
{
  zdebug() << "Played Song:" << song << "Priority:" << priority;
}

