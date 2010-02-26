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

    void setSampleRate( int hertz );
    void setBufferLength( int len );
    void setVolume( int volume );

    enum WaveformType {
      None = 0,
      Sine,
      Square,
      Triangle,
      Sawtooth,
      TriSquare
    };

    void setWaveform( WaveformType type );

    void openAudio();
    void closeAudio();

  protected:
    virtual void begin();
    virtual void clear();
    virtual bool hasNotes() const;
    virtual void addNote( bool tone, int key, int ticks );
    virtual void end();

  private:
    SDLMusicStreamPrivate *d;
};

#endif /* __SDL_MUSIC_STREAM_H__ */

