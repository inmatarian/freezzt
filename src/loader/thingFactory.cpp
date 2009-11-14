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

    GameWorld *world;
    GameBoard *board;

  private:
    ThingFactory *self;
};

ThingFactory::ThingFactory()
  : d( new ThingFactoryPrivate(this) )
{
  
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

  return 0;
}

