/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <cassert>
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

static int square_wave[wave_size] = {
  -64, -64, -64, -64, -64, -64, -64, -64, 
  -64, -64, -64, -64, -64, -64, -64, -64,
   64,  64,  64,  64,  64,  64,  64,  64,
   64,  64,  64,  64,  64,  64,  64,  64
};

static int triangle_wave[wave_size] = {
  -64, -56, -48, -40, -32, -24, -16,  -8,
    0,   8,  16,  24,  32,  40,  48,  56,
   64,  56,   8,  -0,  32,  24,  16,   8,
    0,  -8, -16, -24, -32, -40, -48, -56
};

float note_table[] = {
  261.6255653006,  // C
  277.18263097687, // C#
  293.66476791741, // D
  311.12698372208, // D#
  329.62755691287, // E
  349.228231433,   // F
  369.99442271163, // F#
  391.99543598175, // G
  415.30469757995, // G#
  440.0,           // A
  466.16376151809, // A#
  493.88330125612, // B
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

    std::list<char> musicRoll;
    AbstractMusicStream::PriorityType priority;

    Uint32 sample;
    int beat;
};

SDLMusicStreamPrivate::SDLMusicStreamPrivate()
  : begun( false ),
    hertz( 11025 ),
    bufferLen( 1024 ),
    volume( 32 ),
    waveform( SDLMusicStream::Square ),
    sample( 0 ),
    beat( 0 )
{
  /* */
}

SDLMusicStreamPrivate::~SDLMusicStreamPrivate()
{
  /* */
}

void SDLMusicStreamPrivate::playback(Sint8 *stream, int len)
{
  const int whole_note = hertz * 7 / 4;
  if ( beat > whole_note ) {
    return;
  }

  beat += len;
  int beatLeft = whole_note - beat;
  const int newLen = beatLeft > len ? len : beatLeft;

  const float note = note_table[9];
  Uint32 increment = (Uint32) ( note / (float)hertz * 4294967296.0 );
  for ( int i = 0; i < newLen; i++ )
  {
    const int sample_pos = sample >> increment_precision;
    const int sample_val = triangle_wave[ sample_pos & wave_bits ];
    stream[i] += (sample_val * volume) >> 7;
    sample += increment;
  }
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

void SDLMusicStream::begin()
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

void SDLMusicStream::playMusic( const char *song, PriorityType priority )
{
  zdebug() << "Played Song:" << song << "Priority:" << priority;
  SDL_LockAudio();

  if ( d->musicRoll.empty() || priority > d->priority )
  {
    d->musicRoll.clear();
    d->priority = priority;
  }

  // marks a reset point
  d->musicRoll.push_back( -1 );

  int i = 0;
  while ( song[i] )
  {
    d->musicRoll.push_back(song[i]);
    i++;
  }

  SDL_UnlockAudio();
}

void SDLMusicStream::end()
{
  assert( d->begun );
  zinfo() << "SDLMusicStream::end";
  SDL_PauseAudio(1);
  SDL_CloseAudio();
  d->begun = false;
}

