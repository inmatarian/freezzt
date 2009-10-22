
#include "debug.h"
#include "zztEntity.h"
#include "entityManager.h"

class EntityManagerPrivate
{
  public:
    EntityManagerPrivate( EntityManager *pSelf );
    ~EntityManagerPrivate();

    // flyweights
    EmptySpaceEntity *sharedEmptySpace;

  private:
    EntityManager *self;
};

EntityManagerPrivate::EntityManagerPrivate( EntityManager *pSelf )
  : sharedEmptySpace( new EmptySpaceEntity() ),
    self(pSelf)
{
  /* */  
}

EntityManagerPrivate::~EntityManagerPrivate()
{
  delete sharedEmptySpace;
  sharedEmptySpace = 0;

  self = 0;
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

ZZTEntity * EntityManager::createEntity( int id )
{
  // no need to create flyweights, save some memory
  switch ( id )
  {
    case 0:
      return d->sharedEmptySpace;

    default: break;
  }
  return d->sharedEmptySpace;
}

void EntityManager::destroyEntity( ZZTEntity * entity )
{
  // don't delete flyweights, they're in use elsewhere
  if ( !entity ||
       entity == d->sharedEmptySpace ) {
    return;
  }

  delete entity;
}

