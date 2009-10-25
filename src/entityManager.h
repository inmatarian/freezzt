// Insert copyright and license information here.

#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

class ZZTEntity;

class EntityManagerPrivate;
class EntityManager
{
  public:
    EntityManager();
    virtual ~EntityManager();

    ZZTEntity *createEntity( unsigned char id, unsigned char color );
    void destroyEntity( ZZTEntity * entity );

  private:
    EntityManagerPrivate *d;
};

#endif // ENTITY_MANAGER_H

