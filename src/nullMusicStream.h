/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef __NULL_MUSIC_STREAM_H__
#define __NULL_MUSIC_STREAM_H__

#include "abstractMusicStream.h"

class NullMusicStream : public AbstractMusicStream
{
  public:
    NullMusicStream() { /* */ };
    virtual ~NullMusicStream() { /* */ };

    void setSampleRate( int hertz ) { /* */ };
    void setBufferLength( int len ) { /* */ };
    void setVolume( int volume ) { /* */ };

    enum WaveformType {
      None = 0,
      Sine,
      Square,
      Triangle,
      Sawtooth,
      TriSquare
    };

    void setWaveform( WaveformType type ) { /* */ };

    void openAudio() { /* */ };
    void closeAudio() { /* */ };

  protected:
    virtual void begin_impl() { /* */ };
    virtual void clear() { /* */ };
    virtual bool hasNotes() const { return false; };
    virtual void addNote( bool tone, int key, int ticks ) { /* */ };
    virtual void end_impl() { /* */ };
};

#endif /* __NULL_MUSIC_STREAM_H__ */


