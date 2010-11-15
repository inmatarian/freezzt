/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <list>
#include <vector>
#include <string>
#include <cassert>

#include "debug.h"
#include "zstring.h"
#include "defines.h"
#include "gameBoard.h"
#include "gameWorld.h"
#include "abstractPainter.h"
#include "abstractMusicStream.h"
#include "zztEntity.h"
#include "zztThing.h"
#include "scriptable.h"
#include "zztoopInterp.h"
#include "player.h"

const int FIELD_SIZE = 1500;

typedef std::list<ZZTThing::AbstractThing*> ThingList;
typedef std::list<ZZTOOP::Interpreter*> ProgramsList;
typedef std::vector<ZZTEntity> Field;

class GameBoardPrivate
{
  public:
    GameBoardPrivate( GameBoard *pSelf );
    virtual ~GameBoardPrivate();

    void collectGarbage();
    void drawMessageLine( AbstractPainter *painter );

  public:
    GameWorld *world;

    Field field;

    ThingList thingList;
    ThingList thingGarbage;

    ProgramsList programs;

    unsigned int boardCycle;

    ZString message;
    int messageLife;

    int northExit;
    int southExit;
    int westExit;
    int eastExit;
    bool darkness;

  private:
    GameBoard *self;
};

GameBoardPrivate::GameBoardPrivate( GameBoard *pSelf )
  : world(0),
    boardCycle(0),
    messageLife(0),
    northExit(0),
    southExit(0),
    westExit(0),
    eastExit(0),
    darkness(false),
    self( pSelf )
{
  field.resize(FIELD_SIZE);
}

GameBoardPrivate::~GameBoardPrivate()
{
  // clean out the thing list
  while ( !thingList.empty() ) {
    ZZTThing::AbstractThing *thing = thingList.front();
    thingList.pop_front();
    delete thing;
  }

  collectGarbage();

  while ( !programs.empty() ) {
    ZZTOOP::Interpreter *prog = programs.front();
    programs.pop_front();
    delete prog;
  }

  self = 0;
}

void GameBoardPrivate::collectGarbage()
{
  while ( !thingGarbage.empty() ) {
    ZZTThing::AbstractThing *thing = thingGarbage.front();
    thingGarbage.pop_front();
    delete thing;
  }
}

void GameBoardPrivate::drawMessageLine( AbstractPainter *painter )
{
  if ( messageLife <= 0 ) return;

  int x = 29 - ( message.size() / 2 );
  if ( x < 0 ) x = 0;
  const int color = (boardCycle % 8) + Defines::DARK_GRAY;

  painter->drawText( x, 24, color, message );
}

static int fieldHash( int x, int y )
{
  return (y * 60) + x;
}

// I'm too lazy to do it any other way
static const char torchShape[9][16] = {
"###         ###",
"##           ##",
"#             #",
"#             #",
"       *       ",
"#             #",
"#             #",
"##           ##",
"###         ###"
};

static bool beyondVisible( const int x, const int y, const int px, const int py )
{
  const int dx = 7 + (px - x);
  const int dy = 4 + (py - y);
  if ( dx < 0 || dy < 0 || dx >= 15 || dy >= 9 ) return true;
  return torchShape[dy][dx]=='#';
}

// ---------------------------------------------------------------------------

GameBoard::GameBoard()
  : d( new GameBoardPrivate(this) )
{

}

GameBoard::~GameBoard()
{
  delete d;
  d = 0;
}

GameBoard *GameBoard::addEmptyBoard( GameWorld *world, int pos )
{
  GameBoard *board = new GameBoard();
  world->addBoard( pos, board );

  ZZTThing::Player *player = new ZZTThing::Player();
  player->setBoard( board );
  player->setPos( 30, 12 );
  player->setCycle( 1 );
  ZZTEntity underEnt = ZZTEntity::createEntity( ZZTEntity::EmptySpace, 0x07 );
  player->setUnderEntity( underEnt );
  ZZTEntity playerEnt = ZZTEntity::createEntity( ZZTEntity::Player, 0x1f );
  playerEnt.setThing( player );

  board->setEntity( 30, 12, playerEnt );
  board->addThing( player );

  for ( int x = 0; x < 60; x++ ) {
    board->setEntity( x,  0, ZZTEntity::createEntity( ZZTEntity::Normal, 0x0e ) );
    board->setEntity( x, 24, ZZTEntity::createEntity( ZZTEntity::Normal, 0x0e ) );
  }
  for ( int y = 1; y < 24; y++ ) {
    board->setEntity(  0, y, ZZTEntity::createEntity( ZZTEntity::Normal, 0x0e ) );
    board->setEntity( 59, y, ZZTEntity::createEntity( ZZTEntity::Normal, 0x0e ) );
  }

  return board;
}

void GameBoard::setWorld( GameWorld *world )
{
  d->world = world;
}

GameWorld * GameBoard::world() const
{
  return d->world;
}

AbstractMusicStream *GameBoard::musicStream() const
{
  return world()->musicStream();
}

void GameBoard::clear()
{
  for ( int x = 0; x < FIELD_SIZE; x++ ) {
    d->field[x] = ZZTEntity();
  }
}

const ZZTEntity & GameBoard::entity( int x, int y ) const
{
  if ( x < 0 || x >= 60 || y < 0 || y >= 25 ) {
    return ZZTEntity::sharedEdgeOfBoardEntity();
  }
  return d->field[ fieldHash(x, y) ];
}

void GameBoard::setEntity( int x, int y, const ZZTEntity &entity )
{
  if ( x < 0 || x >= 60 || y < 0 || y >= 25 ) {
    return;
  }

  d->field[ fieldHash(x, y) ] = entity;
}

void GameBoard::replaceEntity( int x, int y, const ZZTEntity &newEntity )
{
  if ( x < 0 || x >= 60 || y < 0 || y >= 25 ) {
    return;
  }

  const int index = fieldHash(x, y);
  ZZTThing::AbstractThing *thing = d->field[index].thing();

  if ( thing ) {
    // wipe it from existance
    d->thingList.remove( thing );
    d->thingGarbage.push_back( thing );
    thing = 0;    
  }

  d->field[index] = newEntity;
}

void GameBoard::clearEntity( int x, int y )
{
  replaceEntity( x, y, ZZTEntity::createEntity( ZZTEntity::EmptySpace, 0x07 ) );
}

void GameBoard::exec()
{
  for ( int i = 0; i<FIELD_SIZE; i++ ) {
    d->field[i].exec();
  }

  // update board cycle
  d->boardCycle += 1;
  d->messageLife -= 1;
  d->collectGarbage();
}

void GameBoard::paint( AbstractPainter *painter )
{
  // copy thing characters out to entities
  ThingList::iterator iter;
  for( iter = d->thingList.begin(); iter != d->thingList.end(); ++iter )
  {
    ZZTThing::AbstractThing *thing = *iter;
    thing->updateEntity();
  }

  ZZTThing::Player *plyr = player();
  const int px = plyr->xPos();
  const int py = plyr->yPos();

  // paint all entities
  for ( int i = 0; i<FIELD_SIZE; i++ )
  {
    const int x = (i%60);
    const int y = (i/60);

    if ( d->world->transitionTile(x, y) ) {
      // don't paint over transition tiles
      continue;
    }

    ZZTEntity &entity = d->field[i];

    if ( isDark() &&
         !entity.isVisibleInDarkness() &&
         ( world()->currentTorchCycles() == 0 ||
           beyondVisible( x, y, px, py ) ) )
    {
      painter->paintChar( x, y, 0xb0, 0x07 );
      continue;
    }

    entity.paint( painter, x, y );
  }

  d->drawMessageLine( painter );
}

const ZString & GameBoard::message() const
{
  return d->message;
}

void GameBoard::setMessage( const ZString &mesg )
{
  d->message.clear();
  if ( mesg.empty() ) {
    d->messageLife = 0;
  }
  else {
    d->message += ' ';
    d->message += mesg;
    d->message += ' ';
    d->messageLife = 90;
  }
}

int GameBoard::northExit() const { return d->northExit; }
int GameBoard::southExit() const { return d->southExit; }
int GameBoard::westExit() const { return d->westExit; } 
int GameBoard::eastExit() const { return d->eastExit; }

void GameBoard::setNorthExit( int exit ) { d->northExit = exit; }
void GameBoard::setSouthExit( int exit ) { d->southExit = exit; }
void GameBoard::setWestExit( int exit ) { d->westExit = exit; }
void GameBoard::setEastExit( int exit ) { d->eastExit = exit; }

bool GameBoard::isDark() const { return d->darkness; }
void GameBoard::setDark( bool dark ) { d->darkness = dark; };

void GameBoard::addThing( ZZTThing::AbstractThing *thing )
{
  d->thingList.push_back( thing );

  ZZTEntity &ent = d->field[ fieldHash( thing->xPos(), thing->yPos() ) ];
  ent.setThing( thing );

  thing->updateEntity();
}

void GameBoard::addInterpreter( ZZTOOP::Interpreter *interp )
{
  // Gameboard owns the programs, not the Scriptable Objects
  d->programs.push_back( interp );

#if 0
  zdebug() << "GameBoard::addProgramBank" << prog << prog->length();
  zout() << "\n";
  for ( signed short i = 0; i < prog->length(); i++ ) {
    signed char c = (*prog)[i];
    if ( c == 0x0d ) zout() << " \\n ";
    else if ( c >= ' ' ) zout() << ZString(1, c);
    else zout() << "?";
  }

  zout() << "\n";
#endif
}

void GameBoard::moveThing( ZZTThing::AbstractThing *thing, int newX, int newY )
{
  if ( newX < 0 || newX >= 60 || newY < 0 || newY >= 25 ) {
    return;
  }

  // get thing's entity
  ZZTEntity thingEnt = d->field[ fieldHash( thing->xPos(), thing->yPos() ) ];

  // get the neighbor's entity
  ZZTEntity newUnderEnt = d->field[ fieldHash(newX, newY) ];

  // restore what was there before him.
  d->field[ fieldHash( thing->xPos(), thing->yPos() ) ] = thing->underEntity();

  // push neighbor underneath him
  thing->setUnderEntity( newUnderEnt );

  // put thing's entity in the new spot
  d->field[ fieldHash(newX, newY) ] = thingEnt;

  // move to new spot
  thing->setPos( newX, newY );
}

void GameBoard::switchThings( ZZTThing::AbstractThing *left,
                              ZZTThing::AbstractThing *right )
{
  // get their positions
  int lx = left->xPos(),
      ly = left->yPos(),
      rx = right->xPos(),
      ry = right->yPos();

  // get things entities
  ZZTEntity leftEnt =  d->field[ fieldHash(lx, ly) ];
  ZZTEntity rightEnt = d->field[ fieldHash(rx, ry) ];

  // swap them
  d->field[ fieldHash(lx, ly) ] = rightEnt;
  d->field[ fieldHash(rx, ry) ] = leftEnt;

  // get their underneaths
  leftEnt = left->underEntity();
  rightEnt = right->underEntity();

  // swap them
  left->setUnderEntity( rightEnt );
  right->setUnderEntity( leftEnt );
  
  // now swap positions
  left->setPos( rx, ry );
  right->setPos( lx, ly );
}

void GameBoard::makeBullet( int x, int y, int x_step, int y_step, bool playerType )
{
  if ( x < 0 || x >= 60 || y < 0 || y >= 25 ) {
    return;
  }

  ZZTEntity ent = d->field[ fieldHash(x,y) ];
  if ( ent.isWalkable() || ent.isSwimable() )
  {
    // create a bullet
    ZZTThing::Bullet *bullet = new ZZTThing::Bullet;
    bullet->setXPos( x );
    bullet->setYPos( y );
    bullet->setDirection( x_step, y_step );
    bullet->setType( playerType );
    bullet->setBoard( this );
    bullet->setUnderEntity( ent );
    bullet->setCycle( 1 );
    ZZTEntity bulletEnt = ZZTEntity::createEntity( ZZTEntity::Bullet, 0x0f );
    bulletEnt.setThing( bullet );
    d->field[ fieldHash(x,y) ] = bulletEnt;
    d->thingList.push_back( bullet );
  }
  else {
    // simulate a bullet
    handleBulletCollision( x, y, x_step, y_step, playerType );
  }
}

void GameBoard::handleBulletCollision( int x, int y, int x_step, int y_step, bool playerType )
{
  ZZTEntity ent = entity( x, y );
  if ( playerType )
  {
    switch( ent.id() ) {
      case ZZTEntity::Object:
        // send :shot message
        break;
      case ZZTEntity::Breakable:
      case ZZTEntity::Gem:
        clearEntity( x, y );
        musicStream()->playEvent( AbstractMusicStream::Breakable );
        break;

      case ZZTEntity::Bear:
      case ZZTEntity::Ruffian:
      case ZZTEntity::Slime:
      case ZZTEntity::Shark:
      case ZZTEntity::Lion:
      case ZZTEntity::Tiger:
      case ZZTEntity::CentipedeHead:
      case ZZTEntity::CentipedeSegment:
        deleteThing( ent.thing() );
        musicStream()->playEvent( AbstractMusicStream::KillEnemy );
        break;

      default: break;
    }
  }
  else
  {
    switch( ent.id() ) {
      case ZZTEntity::Player:
        // hurt player
        break;
      default: break;
    }
  }
}

void GameBoard::makeStar( int x, int y )
{
  if ( x < 0 || x >= 60 || y < 0 || y >= 25 ) {
    return;
  }

  ZZTEntity ent = d->field[ fieldHash(x,y) ];
  if ( ! ( ent.isWalkable() || ent.isSwimable() ) ) {
    return;
  }

  ZZTThing::Star *star = new ZZTThing::Star;
  star->setXPos( x );
  star->setYPos( y );
  star->setBoard( this );
  star->setUnderEntity( ent );
  star->setCycle( 1 );
  ZZTEntity starEnt = ZZTEntity::createEntity( ZZTEntity::Star, 0x0f );
  starEnt.setThing( star );
  d->field[ fieldHash(x,y) ] = starEnt;
  d->thingList.push_back( star );
}

void GameBoard::deleteThing( ZZTThing::AbstractThing *thing )
{
  thing->handleDeleted();
  d->thingList.remove( thing );
  d->thingGarbage.push_back( thing );
  d->field[ fieldHash(thing->xPos(), thing->yPos()) ] = thing->underEntity();
}

unsigned int GameBoard::cycle() const
{
  return d->boardCycle;
}

ZZTThing::Player *GameBoard::player() const
{
  assert( d->thingList.size() > 0 );

  ZZTThing::Player *player =
    dynamic_cast<ZZTThing::Player *>(d->thingList.front());

  assert( player );

  return player;
}

void GameBoard::pushEntities( int x, int y, int x_step, int y_step )
{
  if ( entity( x, y ).isWalkable() ) {
    return;
  }

  // only push cardinal directions, not in diagonals or idle
  if ( ! ( ( x_step == 0 && y_step != 0 ) ||
           ( x_step != 0 && y_step == 0 ) ) ) {
    return;
  }

  // normalize to 1 or -1
  x_step = ( x_step > 0 ) ?  1
         : ( x_step < 0 ) ? -1 : 0;

  // normalize to 1 or -1
  y_step = ( y_step > 0 ) ?  1
         : ( y_step < 0 ) ? -1 : 0;

  // iterate through pushables until we find a walkable
  int tx = x, ty = y;
  int pushCount = 0;
  while (true)
  {
    const ZZTEntity &ent = entity( tx, ty );
    if ( ent.isWalkable() ) {
      break;
    }

    if ( !ent.isPushable( x_step, y_step ) ) {
      return;
    }

    tx += x_step;
    ty += y_step;
    pushCount += 1;
  }

  // okay, we're at a walkable. copy everything back now.
  int px = tx, py = ty;
  while ( tx != x || ty != y )
  {
    px -= x_step;
    py -= y_step;

    if ( entity( px, py ).isThing() ) {
      moveThing( entity( px, py ).thing(), tx, ty );
    }
    else {
      ZZTEntity p_ent = entity( px, py );
      setEntity( tx, ty, p_ent );
    }

    tx = px;
    ty = py;
  }

  if ( pushCount >= 1 ) {
    musicStream()->playEvent( AbstractMusicStream::Push );
  }

  setEntity( x, y, ZZTEntity::createEntity( ZZTEntity::EmptySpace, 0x07 ) );
}

void GameBoard::sendLabel( const ZString &to, const ZString &label,
                           const ZZTThing::AbstractThing *from )
{
  ZString upperTo = to.upper();
  const bool toAll = ( upperTo == "ALL" );
  const bool toOthers = ( upperTo == "OTHERS" );

  ThingList::iterator iter;
  for( iter = d->thingList.begin(); iter != d->thingList.end(); ++iter )
  {
    ZZTThing::AbstractThing *thing = *iter;
    if ( thing->entityID() != ZZTEntity::Object )
      continue;

    ZZTThing::ScriptableThing *object = dynamic_cast<ZZTThing::ScriptableThing*>(thing);
    const ZString upperName = object->objectName().upper();

    if ( toAll || upperTo == upperName ||
         (toOthers && (from != thing)) )
    {
      object->seekLabel( label );
    }
  }
}

