/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <list>
#include <string>
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

enum GameState
{
  InitState,
  ConfigState,
  MenuState,
  TransitionState,
  TitleState,
  PlayState,
  PauseState,
  GameOverState,
  QuitState,
  max_state
};

class FreeZZTManagerPrivate
{
  public:
    FreeZZTManagerPrivate( FreeZZTManager *pSelf );
    ~FreeZZTManagerPrivate();

    void loadSettings();
    void drawPlainWorldFrame();
    void drawTitleWorldFrame();
    void drawPlayWorldFrame();
    void drawTitleInfoBar();
    void drawPlayInfoBar();

    void doFramerateDelay();

    void setState( GameState newState );
    void runTransitionState();

    void doFractal( int index, int &x, int &y );

  public:
    DotFileParser dotFile;
    GameWorld *world;
    int frames;
    int frameRate;
    int lastFrameClock;
    int lastKey;

    int transitionPrime;
    int transitionNextBoard;
    int transitionClock;

    AbstractPainter *painter;
    AbstractEventLoop *eventLoop;

    GameState gameState;
    GameState nextState;

  private:
    FreeZZTManager *self;
};

FreeZZTManagerPrivate::FreeZZTManagerPrivate( FreeZZTManager *pSelf )
  : world(0),
    frames(0),
    frameRate(30),
    lastFrameClock(0),
    lastKey(0),
    transitionPrime(1),
    transitionNextBoard(0),
    painter(0),
    eventLoop(0),
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

  delete painter;
  painter = 0;

  delete eventLoop;
  eventLoop = 0;
}

void FreeZZTManagerPrivate::loadSettings()
{
  zinfo() << "Parsing dotfile";

  // declare settings keys
  dotFile.addKey("framerate");
  dotFile.addKey("transition_prime");

  // load keys
  dotFile.load("freezztrc");

  // get frameRate
  frameRate = dotFile.getInt( "framerate", 1 );
  if ( frameRate < 1 ) frameRate = 1;
  else if ( frameRate > 60 ) frameRate = 60;
  zdebug() << "frameRate:" << frameRate;

  // get transitionPrime
  transitionPrime = dotFile.getInt( "transition_prime", 1 );
  zdebug() << "transitionPrime:" << transitionPrime;
}

void FreeZZTManagerPrivate::drawPlainWorldFrame()
{
  painter->begin();
  world->paint( painter );
  painter->end();
  doFramerateDelay();
}

void FreeZZTManagerPrivate::drawTitleWorldFrame()
{
  painter->begin();
  world->paint( painter );
  drawTitleInfoBar();
  painter->end();
  doFramerateDelay();
}

void FreeZZTManagerPrivate::drawPlayWorldFrame()
{
  painter->begin();
  world->paint( painter );
  drawPlayInfoBar();
  painter->end();
  doFramerateDelay();
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

void FreeZZTManagerPrivate::drawTitleInfoBar()
{
  using namespace Defines;
  const int textColor = BG_BLUE | WHITE;
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
  drawCenteredTextLine( painter, 60, 22, " ", textColor, textColor );
  drawCenteredTextLine( painter, 60, 23, "F....:....S", altButTextColor, textColor );
  drawCenteredTextLine( painter, 60, 24, " ", textColor, textColor );
}

void FreeZZTManagerPrivate::drawPlayInfoBar()
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

void FreeZZTManagerPrivate::doFramerateDelay()
{
  frames++;

  // the user set framerate is for play only
  const int useFrameRate = ( gameState == TitleState ||
                             gameState == PlayState )
                           ? frameRate
                           : 50;

  const int clock = eventLoop->clock();
  const int delay = 1000 / useFrameRate;
  
  int nextFrameClock = lastFrameClock + delay;

  if ( nextFrameClock < clock ) {
    // don't try to play catch up, forget it.
    nextFrameClock = clock + delay;
  }

  lastFrameClock = nextFrameClock;

  eventLoop->sleep( nextFrameClock );
}

void FreeZZTManagerPrivate::setState( GameState newState )
{
  if ( newState == gameState ) {
    return;
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

  gameState = newState;
}

void FreeZZTManagerPrivate::runTransitionState()
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

  if ( transitionClock % 75 == 0 ) {
    drawPlainWorldFrame();
  }

  transitionClock += 1;
}

void FreeZZTManagerPrivate::doFractal( int index, int &x, int &y )
{
  int k = index * transitionPrime % 1500;
  x = k % 60;
  y = k / 60;
}

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

void FreeZZTManager::setFrameRate( int hertz )
{
  d->frameRate = hertz;
}

int FreeZZTManager::frameRate() const
{
  return d->frameRate;
}

void FreeZZTManager::setPainter( AbstractPainter *painter )
{
  d->painter = painter;
}

void FreeZZTManager::setEventLoop( AbstractEventLoop *eventLoop )
{
  d->eventLoop = eventLoop;
}

void FreeZZTManager::doKeypress( int keycode, int unicode )
{
  using namespace Defines;

  if ( keycode == Z_F10 ) {
    // during development, F10 is the auto-quit key
    d->eventLoop->stop();
    d->nextState = QuitState;
  }

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
        default: break;
      }

    case PlayState:
      d->world->addInputKey( keycode, unicode );
      break;

    case PauseState:      break;
    case GameOverState:   break;
    default: break;
  }
}

/// event loop triggers frame update
void FreeZZTManager::doFrame()
{
  switch ( d->gameState )
  {
    case ConfigState:     d->nextState = MenuState; break;
    case MenuState:       d->nextState = TitleState; break;
    case TransitionState: d->runTransitionState(); break;

    case TitleState:      
      d->world->exec();
      d->drawTitleWorldFrame();
      break;

    case PlayState:
      d->world->setCurrentTimePassed( d->world->currentTimePassed() + 1 );
      d->world->exec();
      d->drawPlayWorldFrame();
      break;

    case PauseState:      d->drawPlainWorldFrame(); break;
    case GameOverState:   d->nextState = TitleState; break;
    default: break;
  }

  d->setState( d->nextState );
}

void FreeZZTManager::exec()
{
  if (!d->world) {
    zwarn() << "No world loaded!";
    return;
  }

  int startTime = d->eventLoop->clock();

  d->nextState = TitleState;
  d->gameState = TitleState;

  zinfo() << "Entering event loop.";
  d->eventLoop->exec();

  int endTime = d->eventLoop->clock();
  zinfo() << "Frames:" << d->frames 
          << " Framerate:" << (d->frames*1000)/(endTime-startTime);
}

