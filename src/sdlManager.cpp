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
#include "archUtils.h"

#include "debug.h"
#include "zstring.h"
#include "abstractPainter.h"
#include "screenPainter.h"
#include "simplePainter.h"
#include "openglPainter.h"
#include "qualityglPainter.h"
#include "sdlEventLoop.h"
#include "abstractMusicStream.h"
#include "abstractFileModelFactory.h"
#include "nullMusicStream.h"
#include "sdlMusicStream.h"
#include "dotFileParser.h"
#include "freezztManager.h"
#include "fileListModel.h"
#include "gameWorld.h"

#include "sdlManager.h"

// ---------------------------------------------------------------------------

inline int boundInt( const int left, const int value, const int right )
{
  return ( value < left ) ? left
       : ( value > right) ? right
                          : value;
}

// ---------------------------------------------------------------------------

class NormalFileModelFactory : public AbstractFileModelFactory
{
  private:
    FileListModel fileModel;

  public: 
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
    void createPainter();
    void setScreen( int w, int h, bool full );
    void setKeyboardRepeatRate();
    void openJoystick();
    void closeJoystick();

  public:
    FreeZZTManager *pFreezztManager;
    DotFileParser dotFile;
    ScreenPainter *painter;
    SDL_Surface *display;
    SDL_Joystick *joystick;

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
    joystick(0),
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
  }
  ready = true;
}

void SDLManagerPrivate::loadSettings()
{
  std::string location = ArchUtils::findConfigFile("freezztrc");
  zinfo() << "Parsing dotfile:" << location;

  // load keys
  dotFile.load(location);
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

void SDLManagerPrivate::createPainter()
{
  std::list<std::string> varList;
  varList.push_back("simple");
  varList.push_back("opengl");
  varList.push_back("qualitygl");

  switch ( dotFile.getFromList( "video.painter", 1, varList ) ) {
    case 1: painter = new OpenGLPainter(); break;
    case 2: painter = new QualityGLPainter(); break;
    case 0:
    default:
      painter = new SimplePainter(); break;
  }
}

void SDLManagerPrivate::setScreen( int w, int h, bool full )
{
  display = painter->createWindow( w, h, full );
  if (!display) return;
  painter->setSDLSurface( display );
}

void SDLManagerPrivate::setKeyboardRepeatRate()
{
  int delay = SDL_DEFAULT_REPEAT_DELAY;
  int interval = SDL_DEFAULT_REPEAT_INTERVAL;

  ZString delayStr = dotFile.getValue("keyboard.repeat_delay", 1);
  if ( delayStr.upper() != "DEFAULT" ) {
    int val = dotFile.getInt("keyboard.repeat_delay", 1, delay);
    if ( val > 0 ) delay = val;
  }

  ZString intervalStr = dotFile.getValue("keyboard.repeat_interval", 1);
  if ( intervalStr.upper() != "DEFAULT" ) {
    int val = dotFile.getInt("keyboard.repeat_interval", 1, interval);
    if ( val > 0 ) interval = val;
  }

  zdebug() << "SetKeyboardRepeatRate delay" << delay
           << "interval" << interval;

  SDL_EnableKeyRepeat( delay, interval );
}

void SDLManagerPrivate::openJoystick()
{
  if ( !dotFile.getBool("joystick.enabled", 1, false) ) return;

  if ( SDL_NumJoysticks() <= 0 ) return;

  joystick = SDL_JoystickOpen(0);
  if ( joystick )
    zdebug() << "Opened Joystick:" << SDL_JoystickName(0);
}

void SDLManagerPrivate::closeJoystick()
{
  if ( SDL_NumJoysticks() > 0 && SDL_JoystickOpened(0) )
    SDL_JoystickClose( joystick );
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
  int ret = SDL_Init( SDL_INIT_VIDEO|
                      SDL_INIT_AUDIO|
                      SDL_INIT_TIMER|
                      SDL_INIT_JOYSTICK );
  if(ret==-1) { 
    zerror() << "Could not initialize SDL:" << SDL_GetError();
    return;
  }

  zinfo() << "Creating display surface.";
  d->createPainter();
  d->setScreen( 640, 400, false );
  if (!d->display) return;

  SDL_WM_SetCaption("FreeZZT", "FreeZZT");
  SDL_EnableUNICODE(1);
  d->setKeyboardRepeatRate();
  d->openJoystick();

  // Create music stream
  AbstractMusicStream *musicStream = d->createMusicStream();
  d->pFreezztManager->setMusicStream( musicStream );

  // Create factory
  NormalFileModelFactory fileModelFactory;
  d->pFreezztManager->setFileModelFactory( &fileModelFactory );

  // Load speed from settings
  d->pFreezztManager->setSpeed( d->dotFile.getInt( "speed", 1, 4 ) );

  zinfo() << "Entering event loop";
  SDLEventLoop eventLoop;
  eventLoop.setFrameLatency( d->frameTime );
  eventLoop.setPainter( d->painter );
  eventLoop.setSDLManager( this );
  eventLoop.setZZTManager( d->pFreezztManager );
  eventLoop.exec();

  delete musicStream;
  d->closeJoystick();
  delete d->painter;
}

