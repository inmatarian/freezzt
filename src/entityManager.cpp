
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
  // deleteall flyweightMap
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
    case 0: entity = new EmptySpaceEntity(); break;
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

