/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef THING_FACTORY_H
#define THING_FACTORY_H

class GameWorld;
class GameBoard;

namespace ZZTThing {
  class AbstractThing;
}

class ThingFactoryPrivate;
class ThingFactory
{
  public:
    ThingFactory();

    GameWorld *world() const;
    void setWorld( GameWorld *world );

    GameBoard *board() const;
    void setBoard( GameBoard *board );

    ZZTThing::AbstractThing *createThing( const unsigned char *data, int &thingSize );

  private:
    ThingFactoryPrivate *d;
};

#endif // THING_FACTORY_H

