// Insert copyright and license information here.

#include <map>

#include "debug.h"
#include "zztEntity.h"
#include "entityManager.h"

typedef std::map<int, ZZTEntity*> FlyweightMap;

class EntityManagerPrivate
{
  public:
    EntityManagerPrivate( EntityManager *pSelf );
    ~EntityManagerPrivate();

    FlyweightMap flyweightMap;

    static int entityHash( unsigned char id, unsigned char color );
    ZZTEntity *lookupFlyweightEntity( unsigned char id, unsigned char color );
    void storeFlyweightEntity( unsigned char id, unsigned char color,
                               ZZTEntity *entity );

  private:
    EntityManager *self;
};

EntityManagerPrivate::EntityManagerPrivate( EntityManager *pSelf )
  : self(pSelf)
{
  /* */  
}

EntityManagerPrivate::~EntityManagerPrivate()
{
  zdebug() << "~EntityManagerPrivate";
  FlyweightMap::iterator iter;
  for( iter = flyweightMap.begin(); iter != flyweightMap.end(); ++iter ) {
    delete (*iter).second;
  }

  self = 0;
}

int EntityManagerPrivate::entityHash( unsigned char id,
                                      unsigned char color )
{
  return (((int)id) << 8) + ((int)color);
}

ZZTEntity *EntityManagerPrivate::lookupFlyweightEntity( unsigned char id,
                                                        unsigned char color)
{
  FlyweightMap::iterator it;
  it = flyweightMap.find( entityHash(id, color) );
  if ( it == flyweightMap.end() ) {
    return 0;
  }

  return (*it).second;
}

void EntityManagerPrivate::storeFlyweightEntity( unsigned char id,
                                                 unsigned char color,
                                                 ZZTEntity *entity )
{
  flyweightMap[entityHash(id, color)] = entity;
}

// ---------------------------------------------------------------------------

EntityManager::EntityManager()
  : d( new EntityManagerPrivate(this) )
{
  /* */
}

EntityManager::~EntityManager()
{
  delete d;
  d = 0;
}

ZZTEntity * EntityManager::createEntity( unsigned char id, unsigned char color )
{
  ZZTEntity *entity = d->lookupFlyweightEntity( id, color );
  if (entity) {
    return entity;
  }

  // create entities
  switch ( id )
  {
    case 0x00: entity = new EmptySpaceEntity(); break;
    case 0x14: entity = new ForestEntity(); break;
    case 0x15: entity = new SolidEntity(); break;
    case 0x16: entity = new NormalEntity(); break;
    case 0x17: entity = new BreakableEntity(); break;
    case 0x1B: entity = new FakeEntity(); break;
    default: break;
  }

  if (!entity) {
    return 0;
  }

  entity->setColor( color );

  if ( !entity->isMutable() ) {
    // Objects that aren't allowed to be changed are flyweight friendly.
    d->storeFlyweightEntity( id, color, entity );
  }

  return entity;
}

void EntityManager::destroyEntity( ZZTEntity * entity )
{
  if (!entity->isMutable()) {
    // don't delete flyweights, they're in use elsewhere
    // TODO: Maybe we can reference count them.
    return;
  }

  // safe to delete
  delete entity;
}

