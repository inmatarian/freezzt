/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <SDL.h>
#include "platform.h"

#include "debug.h"
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

    virtual AbstractScrollModel * acquireFileListModel( const std::string &directory = "" )
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

  public:
    FreeZZTManager *pFreezztManager;
    DotFileParser dotFile;
    int frameTime;
    bool audioEnabled;

  private:
    SDLManager *self;
};

SDLManagerPrivate::SDLManagerPrivate( SDLManager *pSelf )
  : pFreezztManager(0),
    frameTime(27),
    audioEnabled(false),
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
}

void SDLManagerPrivate::loadSettings()
{
  zinfo() << "Parsing dotfile";

  // declare settings keys
  dotFile.addKey("frame_time");
  dotFile.addKey("audio_enabled");

  // load keys
  dotFile.load("freezztrc");

  // get frameTime
  frameTime = dotFile.getInt( "frame_time", 1, 27 );
  frameTime = boundInt( 1, frameTime, 1000 );
  zdebug() << "frameTime:" << frameTime;

  audioEnabled = dotFile.getBool( "audio_enabled", 1, false );
  zdebug() << "audio_enabled:" << audioEnabled;
}

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

void SDLManager::exec()
{
  // Initialize defaults, Video and Audio subsystems
  zinfo() << "Initializing SDL.";
  int ret = SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER);
  if(ret==-1) { 
    zerror() << "Could not initialize SDL:" << SDL_GetError();
    return;
  }

  zinfo() << "Creating display surface.";
  SDL_Surface *display = SDL_SetVideoMode( 640, 400, 0, SDL_SWSURFACE );
  if (!display) {
    zerror() << "Could not create display:" << SDL_GetError();
    return;
  }

  SDL_EnableUNICODE(1);

  SDL_WM_SetCaption("FreeZZT", "FreeZZT");
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

  SDL_FillRect( display, 0, 0 );

  d->loadSettings();

  TextmodePainter painter;
  painter.setSDLSurface( display );

  SDLPlatformServices services;

  services.musicStream = d->audioEnabled
                         ? (AbstractMusicStream*) new SDLMusicStream()
                         : (AbstractMusicStream*) new NullMusicStream();

  d->pFreezztManager->setServices( &services );

  zinfo() << "Entering event loop";
  SDLEventLoop eventLoop;
  eventLoop.setFrameLatency( d->frameTime );
  eventLoop.setPainter( &painter );
  eventLoop.setManager( d->pFreezztManager );
  eventLoop.exec();

  d->pFreezztManager->setServices( 0 );
  delete services.musicStream;
}

