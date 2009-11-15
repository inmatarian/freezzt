// Insert copyright and license information here.

#include <string>
#include <memory>

#include "debug.h"

#include "thingFactory.h"
#include "gameWorld.h"
#include "gameBoard.h"
#include "zztEntity.h"
#include "zztThing.h"
#include "zztStructs.h"

class ThingFactoryPrivate
{
  public:
    ThingFactoryPrivate( ThingFactory *pSelf )
      : self( pSelf )
    { /* */ };

    ZZTThing *createThing( const ZZTEntity &entity, const ThingHeader& header );

    Player *createPlayer( const ThingHeader& header );
    Scroll *createScroll( const ThingHeader& header );
    Passage *createPassage( const ThingHeader& header );
    Duplicator *createDuplicator( const ThingHeader& header );
    Bear *createBear( const ThingHeader& header );
    Object *createObject( const ThingHeader& header );
    Slime *createSlime( const ThingHeader& header );
    Shark *createShark( const ThingHeader& header );
    SpinningGun *createSpinningGun( const ThingHeader& header );
    Pusher *createPusher( const ThingHeader& header );
    Lion *createLion( const ThingHeader& header );
    Tiger *createTiger( const ThingHeader& header );
    CentipedeHead *createCentipedeHead( const ThingHeader& header );
    CentipedeSegment *createCentipedeSegment( const ThingHeader& header );
    BlinkWall *createBlinkWall( const ThingHeader& header );
    Transporter *createTransporter( const ThingHeader& header );
    Bullet *createBullet( const ThingHeader& header );
    Star *createStar( const ThingHeader& header );

  public:
    GameWorld *world;
    GameBoard *board;

  private:
    ThingFactory *self;
};

ZZTThing * ThingFactoryPrivate::createThing( const ZZTEntity &entity,
                                             const ThingHeader& header )
{
  ZZTThing *thing = 0;
  switch ( entity.id() )
  {
    case ZZTEntity::Player: thing = createPlayer( header ); break;
    case ZZTEntity::Scroll: thing = createScroll( header ); break;
    case ZZTEntity::Passage: thing = createPassage( header ); break;
    case ZZTEntity::Duplicator: thing = createDuplicator( header ); break;
    case ZZTEntity::Bear: thing = createBear( header ); break;
    case ZZTEntity::Object: thing = createObject( header ); break;
    case ZZTEntity::Slime: thing = createSlime( header ); break;
    case ZZTEntity::Shark: thing = createShark( header ); break;
    case ZZTEntity::SpinningGun: thing = createSpinningGun( header ); break;
    case ZZTEntity::Pusher: thing = createPusher( header ); break;
    case ZZTEntity::Lion: thing = createLion( header ); break;
    case ZZTEntity::Tiger: thing = createTiger( header ); break;
    case ZZTEntity::CentipedeHead: thing = createCentipedeHead( header ); break;
    case ZZTEntity::CentipedeSegment: thing = createCentipedeSegment( header ); break;
    case ZZTEntity::BlinkWall: thing = createBlinkWall( header ); break;
    case ZZTEntity::Transporter: thing = createTransporter( header ); break;
    case ZZTEntity::Bullet: thing = createBullet( header ); break;
    case ZZTEntity::Star: thing = createStar( header ); break;
    default: break;
  }

  if (!thing) return 0;

  thing->setPos( header.x-1, header.y-1 );
  return thing;
}

Player * ThingFactoryPrivate::createPlayer( const ThingHeader& header )
{
  Player *player = new Player();
  return player;
}

Scroll * ThingFactoryPrivate::createScroll( const ThingHeader& header )
{
  Scroll *scroll = new Scroll();
  return scroll;
}

Passage * ThingFactoryPrivate::createPassage( const ThingHeader& header )
{
  Passage *passage = new Passage();
  return passage;
}

Duplicator * ThingFactoryPrivate::createDuplicator( const ThingHeader& header )
{
  Duplicator *duplicator = new Duplicator();
  return duplicator;
}

Bear * ThingFactoryPrivate::createBear( const ThingHeader& header )
{
  return new Bear();
}

Object * ThingFactoryPrivate::createObject( const ThingHeader& header )
{
  return new Object();
}

Slime * ThingFactoryPrivate::createSlime( const ThingHeader& header )
{
  return new Slime();
}

Shark * ThingFactoryPrivate::createShark( const ThingHeader& header )
{
  return new Shark();
}

SpinningGun * ThingFactoryPrivate::createSpinningGun( const ThingHeader& header )
{
  return new SpinningGun();
}

Pusher * ThingFactoryPrivate::createPusher( const ThingHeader& header )
{
  return new Pusher();
}

Lion * ThingFactoryPrivate::createLion( const ThingHeader& header )
{
  return new Lion();
}

Tiger * ThingFactoryPrivate::createTiger( const ThingHeader& header )
{
  return new Tiger();
}

CentipedeHead * ThingFactoryPrivate::createCentipedeHead( const ThingHeader& header )
{
  return new CentipedeHead();
}

CentipedeSegment * ThingFactoryPrivate::createCentipedeSegment( const ThingHeader& header )
{
  return new CentipedeSegment();
}

BlinkWall * ThingFactoryPrivate::createBlinkWall( const ThingHeader& header )
{
  return new BlinkWall();
}

Transporter * ThingFactoryPrivate::createTransporter( const ThingHeader& header )
{
  return new Transporter();
}

Bullet * ThingFactoryPrivate::createBullet( const ThingHeader& header )
{
  return new Bullet();
}

Star * ThingFactoryPrivate::createStar( const ThingHeader& header )
{
  return new Star();
}

// -----------------------------------------------------------------------------

ThingFactory::ThingFactory()
  : d( new ThingFactoryPrivate(this) )
{
  /* */
}

GameWorld *ThingFactory::world() const
{
  return d->world;
}

void ThingFactory::setWorld( GameWorld *world )
{
  d->world = world;
}

GameBoard *ThingFactory::board() const
{
  return d->board;
}

void ThingFactory::setBoard( GameBoard *board )
{
  d->board = board;
}

ZZTThing * ThingFactory::createThing( const unsigned char *data, int &thingSize )
{
  std::auto_ptr<ThingHeader> header( new ThingHeader(data) );

  // Really ZZT file format, I have to look at the board to get the
  // entity number? Why didn't you encode it, you had 7 unused bytes there.
  ZZTEntity entity = d->board->entity( header->x - 1, header->y - 1 );

  thingSize = 0x21 + header->programLength;

  return d->createThing( entity, *header );
}

