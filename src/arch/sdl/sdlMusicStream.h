/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef __SDL_MUSIC_STREAM_H__
#define __SDL_MUSIC_STREAM_H__

#include "abstractMusicStream.h"

class SDLMusicStreamPrivate;
class SDLMusicStream : public AbstractMusicStream
{
  public:
    SDLMusicStream();
    virtual ~SDLMusicStream();

    virtual void playMusic( const char *song, PriorityType priority );

  private:
    SDLMusicStreamPrivate *d;
};

#endif /* __SDL_MUSIC_STREAM_H__ */

