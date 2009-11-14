// Insert copyright and license information here.

#ifndef WORLD_LOADER_H
#define WORLD_LOADER_H

class GameWorld;

class WorldLoaderPrivate;
class WorldLoader
{
  public:
    WorldLoader( const std::string &filename );
    ~WorldLoader();
    bool isValid() const;
    void setWorld( GameWorld *target );
    bool go();

    static GameWorld * loadWorld( const std::string &filename );

  private:
    WorldLoaderPrivate *d;

    // disable copy and assignment
    WorldLoader(const WorldLoader &);
    WorldLoader & operator=(const WorldLoader &);
};

#endif // WORLD_LOADER_H

