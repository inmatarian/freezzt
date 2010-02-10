/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <cassert>
#include <cmath>
#include <list>

#include <SDL.h>

#include "debug.h"
#include "abstractMusicStream.h"
#include "sdlMusicStream.h"

static const int wave_size = 32;
static const int wave_bits = 31;
static const int increment_precision = 27;

/*
static int sine_wave[wave_size] = {
  0, 24, 45, 59, 64, 59, 45, 24, 0, -24, -45, -59, -64, -59, -45, -24
};
*/

static int square_wave( Uint32 val )
{
  return (val&0x80000000) ? -64 : 64;
}

static int triange_wave( Uint32 val )
{
  const int gradient = val >> 24 & 127;
  return (val&0x80000000) ? (-64+gradient) : (64-gradient);
}

static const int MAX_NOTES = 90;
static float note_table[MAX_NOTES];

struct Note
{
  bool isNote;
  float hertz;
  int effect;
  int bytesLeft;
  Uint32 phase;

  Note()
    : isNote( false ),
      hertz(0.0),
      effect(-1),
      bytesLeft(0),
      phase(0)
  { /* */ };

  Note( bool n, float h, int e, int b )
    : isNote(n),
      hertz(h),
      effect(e),
      bytesLeft(b),
      phase(0)
  { /* */ };

  static Note createNote( float hertz, int bytes ) {
    return Note( true, hertz, -1, bytes );
  }

  static Note createEffect( int effect, int bytes ) {
    return Note( false, 0, effect, bytes );
  }

};

// ---------------------------------------------------------------------------

class SDLMusicStreamPrivate
{
  public:
    SDLMusicStreamPrivate();
    ~SDLMusicStreamPrivate();

    static void audio_callback(void *userdata, Uint8 *stream, int len)
    {
      SDLMusicStreamPrivate *p = static_cast<SDLMusicStreamPrivate *>(userdata);
      Sint8 *signed_stream = (Sint8 *) stream;
      p->playback(signed_stream, len);
    }

    void playback(Sint8 *stream, int len);

  public:
    bool begun;
    int hertz;
    int bufferLen;
    int volume;
    SDLMusicStream::WaveformType waveform;

    std::list<Note> noteRoll;
    Note currentNote;

    Uint32 sample;
    int beat;
};

SDLMusicStreamPrivate::SDLMusicStreamPrivate()
  : begun( false ),
    hertz( 44100 ),
    bufferLen( 2048 ),
    volume( 32 ),
    waveform( SDLMusicStream::Square ),
    sample( 0 ),
    beat( 0 )
{
  const float twelvth_root_of_two = pow(2.0, 1.0/12.0);
  for ( int i = 0; i < MAX_NOTES; i++ ) {
    note_table[i] = 440.0 * pow( twelvth_root_of_two, i-49 );
  }
}

SDLMusicStreamPrivate::~SDLMusicStreamPrivate()
{
  /* */
}

void SDLMusicStreamPrivate::playback(Sint8 *stream, int len)
{
  int fill = 0;

  do
  {
    if ( currentNote.bytesLeft <= 0 )
    {
      if ( noteRoll.empty() ) {
        break;
      }
      Uint32 oldPhase = currentNote.phase;
      currentNote = noteRoll.front();
      noteRoll.pop_front();
      if ( currentNote.isNote ) {
        currentNote.phase = oldPhase;
      }
    }

    const int bytes = currentNote.bytesLeft < len ? currentNote.bytesLeft : len;

    if ( currentNote.isNote )
    {
      Uint32 increment = (Uint32) ( currentNote.hertz / (float)hertz * 4294967296.0 );

      for ( int i = 0; i < bytes; i++ )
      {
        const int sample_val = square_wave( currentNote.phase );
        stream[i] += (sample_val * volume) >> 7;
        currentNote.phase += increment;
      }
    }
    else {
      // TODO: Effects
    }

    fill += bytes;
    currentNote.bytesLeft -= bytes;
  }
  while ( fill < len );
}

// ---------------------------------------------------------------------------

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

void SDLMusicStream::setSampleRate( int hertz )
{
  int goodRates[] = { 11025, 22050, 44100 };
  int chosenRate = goodRates[0];
  for ( int i = 0; i < 3; i++ ) {
    if ( hertz < goodRates[i] ) break;
    chosenRate = goodRates[i];
  }

  d->hertz = chosenRate;
}

void SDLMusicStream::setBufferLength( int len )
{
  int goodLen[] = { 512, 1024, 2048, 4096, 8192 };
  int chosenLen = goodLen[0];
  for ( int i = 0; i < 5; i++ ) {
    if ( len < goodLen[i] ) break;
    chosenLen = goodLen[i];
  }

  d->bufferLen = chosenLen;
}

void SDLMusicStream::setVolume( int volume )
{
  d->volume = volume <   0 ? 0
            : volume > 255 ? 255
            : volume;
}

void SDLMusicStream::setWaveform( WaveformType type )
{
  d->waveform = type;
}

void SDLMusicStream::openAudio()
{
  assert( !d->begun );
  d->begun = true;

  zinfo() << "SDLMusicStream::begin" << d->hertz << d->bufferLen
          << d->volume << d->waveform;

  SDL_AudioSpec desired;
  desired.freq = d->hertz; 
  desired.format = AUDIO_S8;
  desired.channels = 1;
  desired.samples = d->bufferLen;
  desired.callback = SDLMusicStreamPrivate::audio_callback;
  desired.userdata = d;

  SDL_OpenAudio( &desired, 0 );
  SDL_PauseAudio(0);
}

void SDLMusicStream::begin()
{
  SDL_LockAudio();
}

void SDLMusicStream::end()
{
  SDL_UnlockAudio();
}

void SDLMusicStream::clear()
{
  /* */
}

bool SDLMusicStream::hasNotes() const
{
  return false;
}

void SDLMusicStream::addNote( bool tone, int key, int ticks )
{
  int bytes = (int)((float) d->hertz / 18.2 * ticks);
  Note note;
  if (tone) {
    int safe_key = ( key >= 0 && key < MAX_NOTES ) ? key : 0;
    note = Note::createNote( note_table[safe_key], bytes );
  }
  else {
    note = Note::createEffect( key, bytes );
  }
  d->noteRoll.push_back( note );
}

void SDLMusicStream::closeAudio()
{
  assert( d->begun );
  zinfo() << "SDLMusicStream::end";
  SDL_PauseAudio(1);
  SDL_CloseAudio();
  d->begun = false;
}

