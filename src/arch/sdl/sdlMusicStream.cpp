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

const float PI = 3.141592653589793;

// ---------------------------------------------------------------------------

class AbstractBufferFiller
{
  public:
    virtual Uint32 fillBuffer( Sint8 *stream, int bytes, int volume,
                               Uint32 phase, Uint32 increment ) = 0;

    static inline Sint8 mix( int stream, int sample, int volume )
    {
      // crude mixer
      const int mix_val = stream + ( ( sample * volume ) >> 7 );
      if ( mix_val < -127 ) return -127;
      if ( mix_val >  127 ) return  127;
      return mix_val;
    }
};

template<typename T>
class TemplateBufferFiller : public AbstractBufferFiller
{
  public:
    virtual Uint32 fillBuffer( Sint8 *stream, int bytes, int volume,
                               Uint32 phase, Uint32 increment )
    {
      for ( int i = 0; i < bytes; i++ ) {
        stream[i] = mix( stream[i], wave.getSample( phase ), volume );
        phase += increment;
      }
      return phase;
    }

  protected:
    T wave;
};

// ---------------------------------------------------------------------------

class SilentWaveform
{
  public:
    static int getSample( Uint32 val ) { return 0; };
};

// -------------------------------------

class SquareWaveform
{
  public: 
    static int getSample( Uint32 val )
    {
      return 64 - ( (val & 0x80000000) >> 25 );
    };
};

// -------------------------------------

class TriangleWaveform
{
  public: 
    static int getSample( Uint32 val )
    {
      const int gradient = (val >> 24) & 127;
      return (val&0x80000000) ? (-64+gradient) : (64-gradient);
    };
};

// -------------------------------------

class SawtoothWaveform
{
  public: 
    static int getSample( Uint32 val )
    {
      const int gradient = (val >> 25) & 127;
      return 64 - gradient;
    };
};

// -------------------------------------

// my own little creation, combines a triangle and a square to sound more tinny.
class TriSquareWaveform
{
  public: 
    static int getSample( Uint32 val )
    {
      switch ( val >> 28 ) {
        case 15: case 0:
          return  64 - ((val >> 21) & 127);
        case 1: case 2: case 3: case 4: case 5: case 6:
          return  64;
        case 7: case 8:
          return -64 + ((val >> 21) & 127);
        case 9: case 10: case 11: case 12: case 13: case 14: default:
          return -64;
      }
    };
};

// -------------------------------------

class SineWaveform
{
  public: 
    SineWaveform()
    {
      if (m_initialized) return;
      m_initialized = true;
      for ( int i = 0; i < 1024; i++ ) {
        m_wave_buffer[i] = (int) ( 64.0 * sin( 2.0*PI * (float)i / 1024.0 ) );
      }
    };

    static int getSample( Uint32 val )
    {
      // the 21st bit here can be considered a 0.5 that we're rounding up from.
      const int index = ( val >> 22 ) + ( (val >> 21) & 1 );
      return m_wave_buffer[ index & 1023 ];
    };

  private:
    static bool m_initialized;
    static signed char m_wave_buffer[1024];
};

bool SineWaveform::m_initialized = false;
signed char SineWaveform::m_wave_buffer[1024];

// ---------------------------------------------------------------------------

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
    SDLMusicStream::WaveformType waveformType;
    AbstractBufferFiller *bufferFiller;
    
    static const int MAX_NOTES = 90;
    static float note_table[MAX_NOTES];

    std::list<Note> noteRoll;
    Note currentNote;
};

float SDLMusicStreamPrivate::note_table[SDLMusicStreamPrivate::MAX_NOTES];

SDLMusicStreamPrivate::SDLMusicStreamPrivate()
  : begun( false ),
    hertz( 44100 ),
    bufferLen( 1024 ),
    volume( 64 ),
    waveformType( SDLMusicStream::Square ),
    bufferFiller( 0 )
{
  // We have to reproduce the pretty bad inaccuracy of the PC speaker here
  // The notes will all be slightly off to account for integer division.
  const float PIT_TIMER = 1193180.0;
  const float twelvth_root_of_two = pow(2.0, 1.0/12.0);

  for ( int i = 0; i < MAX_NOTES; i++ ) {
    float freq = 440.0 * pow( twelvth_root_of_two, i-49 );
    note_table[i] = floor( PIT_TIMER / floor( PIT_TIMER / round( freq ) ) );
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
      if ( noteRoll.empty() ) break;
      Uint32 oldPhase = currentNote.phase;
      currentNote = noteRoll.front();
      noteRoll.pop_front();
      if ( currentNote.isNote ) {
        currentNote.phase = oldPhase;
      }
    }

    const int bytesLeft = len - fill;
    const int bytes = currentNote.bytesLeft < bytesLeft
                    ? currentNote.bytesLeft
                    : bytesLeft;

    if ( currentNote.isNote )
    {
      Uint32 increment = (Uint32) ( currentNote.hertz / (float)hertz * 4294967296.0 );
      currentNote.phase = bufferFiller->fillBuffer( stream+fill, bytes, volume,
                                                    currentNote.phase, increment );
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
  d->waveformType = type;
}

void SDLMusicStream::openAudio()
{
  assert( !d->begun );
  d->begun = true;

  zinfo() << "SDLMusicStream::begin" << d->hertz << d->bufferLen
          << d->volume << d->waveformType;

  switch ( d->waveformType ) {
      case Sine: d->bufferFiller = new TemplateBufferFiller<SineWaveform>; break;
      case Square: d->bufferFiller = new TemplateBufferFiller<SquareWaveform>; break;
      case Triangle: d->bufferFiller = new TemplateBufferFiller<TriangleWaveform>; break;
      case Sawtooth: d->bufferFiller = new TemplateBufferFiller<SawtoothWaveform>; break;
      case TriSquare: d->bufferFiller = new TemplateBufferFiller<TriSquareWaveform>; break;
      case None:
      default: d->bufferFiller = new TemplateBufferFiller<SilentWaveform>; break;
  }

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
    int safe_key = ( key >= 0 && key < d->MAX_NOTES ) ? key : 0;
    note = Note::createNote( d->note_table[safe_key], bytes );
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
  delete d->bufferFiller;
  d->bufferFiller = 0;
}

