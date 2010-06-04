/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <SDL.h>
#include <string>
#include <list>
#include <algorithm>
#include "platform.h"

#include "debug.h"
#include "zstring.h"
#include "abstractPainter.h"
#include "textmodePainter.h"
#include "sdlEventLoop.h"
#include "abstractMusicStream.h"
#include "nullMusicStream.h"
#include "sdlMusicStream.h"
#include "dotFileParser.h"
#include "freezztManager.h"
#include "fileListModel.h"
#include "abstractPlatformServices.h"
#include "sdlManager.h"

// ---------------------------------------------------------------------------

inline int boundInt( const int left, const int value, const int right )
{
  return ( value < left ) ? left
       : ( value > right) ? right
                          : value;
}

// ---------------------------------------------------------------------------

class SDLPlatformServices : public AbstractPlatformServices
{
  public: 
    AbstractMusicStream *musicStream;
    FileListModel fileModel;

  public: 
    virtual AbstractMusicStream * acquireMusicStream() { return musicStream; };
    virtual AbstractMusicStream * currentMusicStream() { return musicStream; };
    virtual void releaseMusicStream( AbstractMusicStream * ) { /* */ };

    virtual AbstractScrollModel * acquireFileListModel( const ZString &directory = "" )
    {
      fileModel.setDirectory(directory);
      return &fileModel;
    };
    virtual AbstractScrollModel * currentFileListModel() { return &fileModel; };
    virtual void releaseFileListModel( AbstractScrollModel *model ) { /* */ };
};

// ---------------------------------------------------------------------------

class SDLManagerPrivate
{
  public:
    SDLManagerPrivate( SDLManager *pSelf );

    void parseArgs( int argc, char ** argv );
    void loadSettings();
    AbstractMusicStream *createMusicStream();
    void setScreen( int w, int h, bool full );

  public:
    FreeZZTManager *pFreezztManager;
    DotFileParser dotFile;
    TextmodePainter painter;
    SDL_Surface *display;

    int frameTime;
    bool ready;

    int windowWidth;
    int windowHeight;
    bool fullscreen;

  private:
    SDLManager *self;
};

SDLManagerPrivate::SDLManagerPrivate( SDLManager *pSelf )
  : pFreezztManager(0),
    display(0),
    frameTime(27),
    ready(false),
    windowWidth( 640 ),
    windowHeight( 400 ),
    fullscreen( false ),
    self(pSelf)
{
  /* */
}

void SDLManagerPrivate::parseArgs( int argc, char ** argv )
{
  if (argc >= 2) {
    zinfo() << "Loading" << argv[1];
    pFreezztManager->loadWorld( argv[1] );
    ready = true;
  }
  else {
    zinfo() << "usage:" << argv[0] << "world.zzt";
  }
}

void SDLManagerPrivate::loadSettings()
{
  zinfo() << "Parsing dotfile";

  // load keys
  dotFile.load("freezztrc");

  // get frameTime
  frameTime = dotFile.getInt( "video.frame_time", 1, 27 );
  frameTime = boundInt( 1, frameTime, 1000 );
  zdebug() << "frameTime:" << frameTime;
}

AbstractMusicStream * SDLManagerPrivate::createMusicStream()
{
  bool audioEnabled = dotFile.getBool( "audio.enabled", 1, false );
  zdebug() << "audio.enabled:" << audioEnabled;

  if (!audioEnabled) {
    return new NullMusicStream();
  }

  SDLMusicStream *stream = new SDLMusicStream();
  stream->setSampleRate( dotFile.getInt( "audio.freq", 1, 22050 ) );
  stream->setBufferLength( dotFile.getInt( "audio.buffer", 1, 2048 ) );
  stream->setVolume( dotFile.getInt( "audio.volume", 1, 64 ) );

  std::list<std::string> varList;
  varList.push_back("None");
  varList.push_back("Sine");
  varList.push_back("Square");
  varList.push_back("Triangle");
  varList.push_back("Sawtooth");
  varList.push_back("TriSquare");

  SDLMusicStream::WaveformType wave = SDLMusicStream::Square;
  switch ( dotFile.getFromList( "audio.wave", 1, varList ) ) {
    case 0: wave = SDLMusicStream::None; break;
    case 1: wave = SDLMusicStream::Sine; break;
    case 2: wave = SDLMusicStream::Square; break;
    case 3: wave = SDLMusicStream::Triangle; break;
    case 4: wave = SDLMusicStream::Sawtooth; break;
    case 5: wave = SDLMusicStream::TriSquare; break;
    default: break;
  }
  stream->setWaveform( wave );

  stream->openAudio();
  return stream;
}

void SDLManagerPrivate::setScreen( int w, int h, bool full )
{
  Uint32 surfaceFlags = SDL_RESIZABLE | SDL_SWSURFACE;
  if ( full ) {
    surfaceFlags |= SDL_FULLSCREEN;
  }
  else {
    windowWidth = w;
    windowHeight = h;
  }

  display = SDL_SetVideoMode( w, h, 0, surfaceFlags );

  if (!display) {
    zerror() << "Could not create display:" << SDL_GetError();
  }
  else {
    SDL_FillRect( display, 0, 0 );
  }

  painter.setSDLSurface( display );
}

// -------------------------------------

SDLManager::SDLManager( int argc, char ** argv )
  : d( new SDLManagerPrivate(this) )
{
  d->pFreezztManager = new FreeZZTManager;
  d->parseArgs( argc, argv );
}

SDLManager::~SDLManager()
{
  delete d->pFreezztManager;
  d->pFreezztManager = 0;

  zinfo() << "Quitting SDL.";
  SDL_Quit();

  delete d;
  d = 0;
}

bool SDLManager::valid() const
{
  return d->ready;
}

void SDLManager::doResize( int w, int h )
{
  d->setScreen( w, h, false );
}

void SDLManager::toggleFullScreen()
{
  d->fullscreen = !d->fullscreen;
  if ( d->fullscreen ) {
    d->setScreen( 640, 400, true );
  }
  else {
    d->setScreen( d->windowWidth, d->windowHeight, false );
  }
}

void SDLManager::exec()
{
  d->loadSettings();

  // Initialize defaults, Video and Audio subsystems
  zinfo() << "Initializing SDL.";
  int ret = SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER);
  if(ret==-1) { 
    zerror() << "Could not initialize SDL:" << SDL_GetError();
    return;
  }

  zinfo() << "Creating display surface.";
  d->setScreen( 640, 400, false );
  if (!d->display) return;

  SDL_WM_SetCaption("FreeZZT", "FreeZZT");
  SDL_EnableUNICODE(1);
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

  d->painter.setSDLSurface( d->display );

  SDLPlatformServices services;
  services.musicStream = d->createMusicStream();
  d->pFreezztManager->setServices( &services );

  d->pFreezztManager->setSpeed( d->dotFile.getInt( "speed", 1, 4 ) );

  zinfo() << "Entering event loop";
  SDLEventLoop eventLoop;
  eventLoop.setFrameLatency( d->frameTime );
  eventLoop.setPainter( &d->painter );
  eventLoop.setSDLManager( this );
  eventLoop.setZZTManager( d->pFreezztManager );
  eventLoop.exec();

  d->pFreezztManager->setServices( 0 );
  delete services.musicStream;
}

