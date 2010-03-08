/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <list>
#include <string>
#include <algorithm>
#include <cassert>

#include "freezztManager.h"

#include "debug.h"
#include "defines.h"
#include "dotFileParser.h"
#include "abstractPainter.h"
#include "gameWorld.h"
#include "gameBoard.h"
#include "worldLoader.h"
#include "abstractEventLoop.h"
#include "abstractMusicStream.h"
#include "abstractPlatformServices.h"

enum GameState
{
  InitState,
  ConfigState,
  MenuState,
  TransitionState,
  TitleState,
  PickSpeedState,
  PlayState,
  PauseState,
  CheatState,
  GameOverState,
  QuitState,
  max_state
};

// ---------------------------------------------------------------------------

class FramerateSliderWidget
{
  public:
    FramerateSliderWidget() 
      : m_value(4) { /* */ };

    void handleKeyPress( int keypress )
    {
      using namespace Defines;
      switch ( keypress ) {
        case Z_Up:
        case Z_Left:
          m_value -= 1;
          break;
        case Z_Down:
        case Z_Right:
          m_value += 1;
          break;
        default: break;
      }
      m_value = boundInt( 0, m_value, 8 );
    };

    int value() const { return m_value; };
    const char *str() const { return strTable[m_value]; };

  private:
    int m_value;
    static const char *strTable[11];
};

const char *FramerateSliderWidget::strTable[11] = {
  "v        ", " v       ", "  v      ", "   v     ", "    v    ",
  "     v   ", "      v  ", "       v ", "        v",
};

// ---------------------------------------------------------------------------

class TextInputWidget
{
  public:
    TextInputWidget() { /* */ };

    void setFixedSuffix( const std::string &sufx ) { /* */ };

    void handleKeyPress( int keypress, int unicode )
    {
      using namespace Defines;
      switch ( keypress ) {
        case Z_Backspace: {
          const int len = userMessage.length();
          if (len > 0) {
            userMessage.erase( len-1, 1 );
          }
          break;
        }
        case Z_Unicode: {
          const unsigned int userMaxLen = ( fixedSuffix.length() > 0 ) ? 8 : 12;
          const char ascii = ( unicode >= 0x20 && unicode <= 0xff ) ? unicode : 0;
          if ( userMessage.length() < userMaxLen && ascii ) {
            userMessage += ascii;
          }
          break;
        }
          
        default: break;
      }
    };

    std::string value() const { return userMessage + fixedSuffix; };
    std::string display() const
    {
      std::string v = userMessage;
      v += fixedSuffix;
      while ( v.length() < 12 ) v.push_back(' ');
      return v;
    };

    void reset() { userMessage.erase(); fixedSuffix.erase(); };

  private:
    std::string userMessage;
    std::string fixedSuffix;
};

// ---------------------------------------------------------------------------

class FreeZZTManagerPrivate
{
  public:
    FreeZZTManagerPrivate( FreeZZTManager *pSelf );
    ~FreeZZTManagerPrivate();

    void loadSettings();
    void drawPlainWorldFrame( AbstractPainter *painter );
    void drawTitleWorldFrame( AbstractPainter *painter );
    void drawPlayWorldFrame( AbstractPainter *painter );
    void drawTitleInfoBar( AbstractPainter *painter );
    void drawPlayInfoBar( AbstractPainter *painter );
    void drawCheatInfoBar( AbstractPainter *painter );
    void drawTextInputWidget( AbstractPainter *painter );

    void doFramerateDelay();

    void setState( GameState newState );
    void runTransitionState( AbstractPainter *painter );

    void doFractal( int index, int &x, int &y );
    void doCheat( const std::string &code );

    void runWorld();

  public:
    DotFileParser dotFile;
    GameWorld *world;
    int debugFrames;
    int frameTime;
    int cycleCountdown;
    bool cycleWorld;

    int transitionPrime;
    int transitionNextBoard;
    int transitionClock;

    AbstractPlatformServices *services;

    GameState gameState;
    GameState nextState;

    FramerateSliderWidget framerateSliderWidget;
    TextInputWidget textInputWidget;

  private:
    FreeZZTManager *self;
};

FreeZZTManagerPrivate::FreeZZTManagerPrivate( FreeZZTManager *pSelf )
  : world(0),
    debugFrames(0),
    frameTime(27),
    cycleCountdown(0),
    cycleWorld(true),
    transitionPrime(1),
    transitionNextBoard(0),
    services(0),
    gameState(InitState),
    nextState(InitState),
    self(pSelf)
{
  /* */
}

FreeZZTManagerPrivate::~FreeZZTManagerPrivate()
{
  delete world;
  world = 0;
}

void FreeZZTManagerPrivate::loadSettings()
{
  zinfo() << "Parsing dotfile";

  // declare settings keys
  dotFile.addKey("frame_time");
  dotFile.addKey("transition_prime");

  // load keys
  dotFile.load("freezztrc");

  // get frameTime
  frameTime = dotFile.getInt( "frame_time", 1, 27 );
  frameTime = boundInt( 1, frameTime, 1000 );
  zdebug() << "frameTime:" << frameTime;

  // get transitionPrime
  transitionPrime = dotFile.getInt( "transition_prime", 1, 29 );
  transitionPrime = boundInt( 1, transitionPrime, (1<<30) );
  zdebug() << "transitionPrime:" << transitionPrime;
}

void FreeZZTManagerPrivate::drawPlainWorldFrame( AbstractPainter *painter )
{
  painter->begin();
  world->paint( painter );
  painter->end();
}

void FreeZZTManagerPrivate::drawTitleWorldFrame( AbstractPainter *painter )
{
  painter->begin();
  world->paint( painter );
  drawTitleInfoBar( painter );
  painter->end();
}

void FreeZZTManagerPrivate::drawPlayWorldFrame( AbstractPainter *painter )
{
  painter->begin();
  world->paint( painter );
  drawPlayInfoBar( painter );
  painter->end();
}

static void drawCenteredTextLine( AbstractPainter *painter, int column, int row,
                                  const std::string &txt, int txtcolor, int spacecolor )
{
  int x = 0;
  int txtln = txt.length();
  int tl = 10 - (txtln / 2);
  int tr = tl + txtln;

  for ( int x=0; x < tl; x++ ) {
    painter->paintChar( column+x, row, ' ', spacecolor );
  }

  painter->drawText( column + tl, row, txtcolor, txt );
  
  for ( int x=tr; x < 20; x++ ) {
    painter->paintChar( column+x, row, ' ', spacecolor );
  }
}

static void drawItemLine( AbstractPainter *painter,
                          int column, int row,
                          int itempic, int itemcolor,
                          const std::string &txt, int txtcolor,
                          int itemval )
{
  using namespace Defines;
  painter->drawText( column, row, txtcolor, "  " );
  painter->paintChar( column+2, row, itempic, itemcolor );
  painter->drawText( column+3, row, txtcolor, txt );
  painter->drawNumber( column+12, row, txtcolor, itemval, 5 );
  painter->drawText( column+17, row, txtcolor, "   " );
}

static void drawKeysLine( AbstractPainter *painter,
                          int column, int row,
                          int itempic, int itemcolor,
                          const std::string &txt, int txtcolor,
                          GameWorld *world )
{
  using namespace Defines;
  int k = 0;
  const int nokey = 0xf9;
  const int bg = itemcolor & 0xF0;
  painter->drawText( column, row, txtcolor, "  " );
  painter->paintChar( column+2, row, itempic, itemcolor );
  painter->drawText( column+3, row, txtcolor, txt );
  k = world->hasDoorKey(GameWorld::BLUE_DOORKEY) ? itempic : nokey;
  painter->paintChar( column+12, row, k, bg | BLUE );
  k = world->hasDoorKey(GameWorld::GREEN_DOORKEY) ? itempic : nokey;
  painter->paintChar( column+13, row, k, bg | GREEN );
  k = world->hasDoorKey(GameWorld::CYAN_DOORKEY) ? itempic : nokey;
  painter->paintChar( column+14, row, k, bg | CYAN );
  k = world->hasDoorKey(GameWorld::RED_DOORKEY) ? itempic : nokey;
  painter->paintChar( column+15, row, k, bg | RED );
  k = world->hasDoorKey(GameWorld::PURPLE_DOORKEY) ? itempic : nokey;
  painter->paintChar( column+16, row, k, bg | MAGENTA );
  k = world->hasDoorKey(GameWorld::YELLOW_DOORKEY) ? itempic : nokey;
  painter->paintChar( column+17, row, k, bg | YELLOW );
  k = world->hasDoorKey(GameWorld::WHITE_DOORKEY) ? itempic : nokey;
  painter->paintChar( column+18, row, k, bg | WHITE );
  painter->paintChar( column+19, row, ' ', txtcolor );
}

static void drawButtonLine( AbstractPainter *painter,
                            int column, int row,
                            const std::string &buttxt, int butcolor,
                            const std::string &valtxt, int txtcolor )
{
  const int butln = buttxt.length();
  const int txtln = valtxt.length();

  painter->drawText( column, row, txtcolor, "  " );
  painter->paintChar( column+2, row, ' ', butcolor );
  painter->drawText( column+3, row, butcolor, buttxt );
  painter->paintChar( column+3+butln, row, ' ', butcolor );
  painter->paintChar( column+4+butln, row, ' ', txtcolor );
  painter->drawText( column+5+butln, row, txtcolor, valtxt );
  int x = 5 + butln + txtln;
  while ( x < 20 ) {
    painter->paintChar( column+x, row, ' ', txtcolor );
    x++;
  }
}

void FreeZZTManagerPrivate::drawTitleInfoBar( AbstractPainter *painter )
{
  using namespace Defines;
  const int textColor = BG_BLUE | WHITE;
  const int blinkTextColor = BLINK | BG_BLUE | WHITE;
  const int altTextColor = BG_BLUE | CYAN;
  const int altButTextColor = BG_BLUE | YELLOW;
  const int buttonColor = BG_GRAY | BLACK;
  const int altColor = BG_CYAN | BLACK;

  drawCenteredTextLine( painter, 60, 0, "- - - - -", textColor, textColor );
  drawCenteredTextLine( painter, 60, 1, "  FREE  ZZT  ", buttonColor, textColor );
  drawCenteredTextLine( painter, 60, 2, "- - - - -", textColor, textColor );
  drawCenteredTextLine( painter, 60, 3, " ", textColor, textColor );
  drawCenteredTextLine( painter, 60, 4, " ", textColor, textColor );
  drawCenteredTextLine( painter, 60, 5, " ", textColor, textColor );
  drawCenteredTextLine( painter, 60, 6, " ", textColor, textColor );
  drawCenteredTextLine( painter, 60, 7, " ", textColor, textColor );
  drawCenteredTextLine( painter, 60, 8, " ", textColor, textColor );
  drawCenteredTextLine( painter, 60, 9, " ", textColor, textColor );
  drawButtonLine( painter, 60, 10, "P", buttonColor, "Play", textColor );
  drawCenteredTextLine( painter, 60,11, " ", textColor, textColor );
  drawCenteredTextLine( painter, 60,12, " ", textColor, textColor );
  drawCenteredTextLine( painter, 60,13, " ", textColor, textColor );
  drawCenteredTextLine( painter, 60,14, " ", textColor, textColor );
  drawCenteredTextLine( painter, 60,15, " ", textColor, textColor );
  drawCenteredTextLine( painter, 60,16, " ", textColor, textColor );
  drawCenteredTextLine( painter, 60,17, " ", textColor, textColor );
  drawCenteredTextLine( painter, 60,18, " ", textColor, textColor );
  drawCenteredTextLine( painter, 60,19, " ", textColor, textColor );
  drawCenteredTextLine( painter, 60,20, " ", textColor, textColor );
  drawButtonLine( painter, 60, 21, "S", buttonColor, "Speed", altButTextColor );

  drawCenteredTextLine( painter, 60, 22, framerateSliderWidget.str(),
                        (gameState == PickSpeedState ? blinkTextColor : textColor),
                        textColor );

  drawCenteredTextLine( painter, 60, 23, "F....:....S", altButTextColor, textColor );
  drawCenteredTextLine( painter, 60, 24, " ", textColor, textColor );
}

void FreeZZTManagerPrivate::drawPlayInfoBar( AbstractPainter *painter )
{
  using namespace Defines;
  const int textColor = BG_BLUE | WHITE;
  const int ammoColor = BG_BLUE | CYAN;
  const int torchColor = BG_BLUE | DARK_BROWN;
  const int buttonColor = BG_GRAY | BLACK;
  const int altColor = BG_CYAN | BLACK;

  drawCenteredTextLine( painter, 60, 0, "- - - - -", textColor, textColor );
  drawCenteredTextLine( painter, 60, 1, "  FREE  ZZT  ", buttonColor, textColor );
  drawCenteredTextLine( painter, 60, 2, "- - - - -", textColor, textColor );
  drawCenteredTextLine( painter, 60, 3, " ", textColor, textColor );
  drawCenteredTextLine( painter, 60, 4, " ", textColor, textColor );
  drawCenteredTextLine( painter, 60, 5, " ", textColor, textColor );
  drawCenteredTextLine( painter, 60, 6, " ", textColor, textColor );
  drawItemLine( painter, 60, 7, 0x02, textColor,  "  Health:", textColor, world->currentHealth() );
  drawItemLine( painter, 60, 8, 0x84, ammoColor,  "    Ammo:", textColor, world->currentAmmo() );
  drawItemLine( painter, 60, 9, 0x9d, torchColor, " Torches:", textColor, world->currentTorches() );
  drawItemLine( painter, 60,10, 0x04, ammoColor,  "    Gems:", textColor, world->currentGems() );
  drawItemLine( painter, 60,11, 0x20, textColor,  "   Score:", textColor, world->currentScore() );
  drawKeysLine( painter, 60,12, 0x0c, textColor,  "    Keys:", textColor, world );
  drawCenteredTextLine( painter, 60, 13, " ", textColor, textColor );
  drawButtonLine( painter, 60, 14, "T", buttonColor, "Torch", textColor );
  drawButtonLine( painter, 60, 15, "B", altColor,    "Be quiet", textColor );
  drawButtonLine( painter, 60, 16, "H", buttonColor, "Help", textColor );
  drawCenteredTextLine( painter, 60, 17, " ", textColor, textColor );
  std::string arrows;
  arrows.append( 1, 0x18 );
  arrows.append( 1, 0x19 );
  arrows.append( 1, 0x1a );
  arrows.append( 1, 0x1b );
  drawButtonLine( painter, 60, 18, arrows, altColor, "Move", textColor );
  std::string shiftArrows = "Shift";
  shiftArrows.append(arrows);
  drawButtonLine( painter, 60, 19, shiftArrows, buttonColor, "Shoot", textColor );
  drawCenteredTextLine( painter, 60, 20, " ", textColor, textColor );
  drawButtonLine( painter, 60, 21, "S", buttonColor, "Save game", textColor );
  drawButtonLine( painter, 60, 22, "P", altColor,    "Pause", textColor );
  drawButtonLine( painter, 60, 23, "Q", buttonColor, "Quit", textColor );
  drawCenteredTextLine( painter, 60, 24, " ", textColor, textColor );
}

void FreeZZTManagerPrivate::drawCheatInfoBar( AbstractPainter *painter )
{
  painter->begin();
  world->paint( painter );
  drawPlayInfoBar( painter );
  drawTextInputWidget( painter );
  painter->end();
}

void FreeZZTManagerPrivate::drawTextInputWidget( AbstractPainter *painter )
{
  using namespace Defines;
  const int textColor = BG_BLUE | WHITE;
  const int widgetColor = WHITE;
  drawCenteredTextLine( painter, 60, 4, textInputWidget.display(), widgetColor, textColor );
}

void FreeZZTManagerPrivate::doFramerateDelay()
{
  if ( cycleCountdown > 0 ) {
    cycleCountdown -= 1;
  }

  if ( cycleCountdown == 0 ) {
    cycleCountdown = framerateSliderWidget.value() + 1;
    cycleWorld = true;
  }

  debugFrames++;
}

void FreeZZTManagerPrivate::setState( GameState newState )
{
  if ( newState == gameState ) {
    return;
  }

  if (gameState == CheatState) {
    doCheat( textInputWidget.value() );
  }

  // ad-hoc transitions
  if ( newState == TransitionState )
  {
    if ( gameState == TitleState ) {
      transitionNextBoard = world->startBoard();
    }
    transitionClock = 0;
  }
  else if ( newState == TitleState )
  {
    GameBoard *board = world->getBoard( 0 );
    world->setCurrentBoard( board );
  }
  else if ( newState == CheatState )
  {
    textInputWidget.reset();
  }

  gameState = newState;
}

void FreeZZTManagerPrivate::runTransitionState( AbstractPainter *painter )
{
  const int transitionSpeed = 80;
  
  for ( int i = 0; i < transitionSpeed; i++ )
  {
    if ( transitionClock < 1500 ) {
      int x, y;
      doFractal( transitionClock, x, y );
      world->setTransitionTile( x, y, true );
    }
    else if ( transitionClock == 1500 ) {
      GameBoard *board = world->getBoard( transitionNextBoard );
      world->setCurrentBoard( board );
    }
    else if ( transitionClock < 3002 ) {
      const int inverse = 1500 - (transitionClock - 1500);
      int x, y;
      doFractal( inverse, x, y );
      world->setTransitionTile( x, y, false );
    }
    else if ( transitionClock == 3002 ) {
      nextState = PlayState;
    }
    transitionClock += 1;
  }

  drawPlainWorldFrame( painter );
}

void FreeZZTManagerPrivate::runWorld()
{
  if (!cycleWorld) return;

  AbstractMusicStream *musicStream = services->currentMusicStream();
  musicStream->begin();
  world->exec();
  musicStream->end();

  cycleWorld = false;
}

void FreeZZTManagerPrivate::doFractal( int index, int &x, int &y )
{
  int k = index * transitionPrime % 1500;
  x = k % 60;
  y = k / 60;
}

void FreeZZTManagerPrivate::doCheat( const std::string &code )
{
  std::string c = code;
  std::transform(c.begin(), c.end(), c.begin(), tolower);
  zdebug() << "CHEAT:" << c;

  if ( c == "ammo" ) { world->setCurrentAmmo( world->currentAmmo() + 5 ); }
  else if ( c == "health" ) { world->setCurrentHealth( world->currentHealth() + 10 ); }
  else if ( c == "gems" ) { world->setCurrentGems( world->currentGems() + 5 ); }
  else if ( c == "torches" ) { world->setCurrentTorches( world->currentTorches() + 5 ); }
  else if ( c == "keys" ) {
    for ( int i = GameWorld::BLUE_DOORKEY; i <= GameWorld::WHITE_DOORKEY; i++ ) {
      world->addDoorKey(i);
    }
  }
}

/*
Clear surrounded by 4 squares     ZAP
Debug mode (OFF)                  +DEBUG
Debug mode (ON)                   -DEBUG
Illuminate room                   -DARK
Obfuscate room                    DARK
*/

// ---------------------------------------------------------------------------

FreeZZTManager::FreeZZTManager()
  : d( new FreeZZTManagerPrivate(this) )
{
  /* */
}

FreeZZTManager::~FreeZZTManager()
{
  delete d;
}

void FreeZZTManager::parseArgs( int argc, char ** argv )
{
  d->loadSettings();

  if (argc >= 2) {
    zinfo() << "Loading" << argv[1];
    d->world = WorldLoader::loadWorld( argv[1] );
  }
  else {
    zinfo() << "Using blank world";
    d->world = new GameWorld();
    GameBoard *board = new GameBoard();
    board->setWorld(d->world);
    d->world->addBoard(0, board);
  }

  d->world->setCurrentBoard( d->world->getBoard(0) );
}

/// Set services
void FreeZZTManager::setServices( AbstractPlatformServices *services )
{
  d->services = services;
}

void FreeZZTManager::doKeypress( int keycode, int unicode )
{
  using namespace Defines;
  AbstractEventLoop *eventLoop = d->services->currentEventLoop();

  switch ( d->gameState )
  {
    case ConfigState:     break;
    case MenuState:       break;
    case TransitionState: break;

    case TitleState:
      switch ( unicode ) {
        case 'P':
        case 'p':
          d->nextState = TransitionState;
          break;
        case 'S':
        case 's':
          d->nextState = PickSpeedState;
          break;
        default: break;
      }
      break; // titlestate

    case PickSpeedState:
      switch ( keycode ) {
        case Z_Enter:
          d->nextState = TitleState;
          break;
        case Z_Unicode:
          switch ( unicode ) {
            case 'S':
            case 's':
              d->nextState = TitleState;
              break;
          }
          break;
        default:
          d->framerateSliderWidget.handleKeyPress(keycode);
          break;
      }
      break; // pickspeedstate

    case PlayState: {
      bool filtered = false;
      switch ( keycode ) {
        case Z_Unicode:
          switch ( unicode ) {
            case 'P':
            case 'p':
              d->nextState = PauseState;
              filtered = true;
              break;
            case '?':
              d->nextState = CheatState;
              filtered = true;
              break;
            default: break;
          }
          break;
        default: break;
      }

      if (!filtered) {
        d->world->addInputKey( keycode, unicode );
      }
      break;
    }

    case PauseState:
      switch ( keycode ) {
        case Z_Unicode:
          switch ( unicode ) {
            case 'P':
            case 'p':
              d->nextState = PlayState;
              break;
            default: break;
          }
          break;
        default: break;
      }
      break;

    case CheatState:
      switch ( keycode ) {
        case Z_Enter:
          d->nextState = PlayState;
          break;
        default:
          d->textInputWidget.handleKeyPress(keycode, unicode);
          break;
      }
      break;

    case GameOverState:   break;
    default: break;
  }
}

/// event loop triggers frame update
void FreeZZTManager::doFrame()
{
  AbstractPainter *painter = d->services->acquirePainter();

  d->doFramerateDelay();

  switch ( d->gameState )
  {
    case ConfigState:     d->nextState = MenuState; break;
    case MenuState:       d->nextState = TitleState; break;
    case TransitionState: d->runTransitionState( painter ); break;

    case TitleState:
      d->runWorld();
      d->drawTitleWorldFrame( painter );
      break;

    case PickSpeedState:
      d->drawTitleWorldFrame( painter );
      break;

    case PlayState:
      d->runWorld();
      d->drawPlayWorldFrame( painter );
      break;

    case CheatState:      d->drawCheatInfoBar( painter ); break;
    case PauseState:      d->drawPlainWorldFrame( painter ); break;
    case GameOverState:   d->nextState = TitleState; break;
    default: break;
  }

  d->setState( d->nextState );

  d->services->releasePainter(painter);
}

void FreeZZTManager::exec()
{
  if (!d->world) {
    zwarn() << "No world loaded!";
    return;
  }

  AbstractMusicStream *musicStream = d->services->acquireMusicStream();
  d->world->setMusicStream( musicStream );

  AbstractEventLoop *eventLoop = d->services->acquireEventLoop();
  int startTime = eventLoop->clock();

  d->nextState = TitleState;
  d->gameState = TitleState;

  zinfo() << "Entering event loop.";
  eventLoop->setFrameLatency( d->frameTime );
  eventLoop->exec();

  int endTime = eventLoop->clock();
  zinfo() << "Frames:" << d->debugFrames 
          << " Framerate:" << (d->debugFrames*1000)/(endTime-startTime);

  d->services->releaseEventLoop( eventLoop );

  d->world->setMusicStream( 0 );
  d->services->releaseMusicStream( musicStream );
}

