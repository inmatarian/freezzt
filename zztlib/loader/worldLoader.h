/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef WORLD_LOADER_H
#define WORLD_LOADER_H

class ZString;
class GameWorld;
class WorldLoaderPrivate;

/// Loads a complete version 3.2 zzt world file into a GameWorld object.
class WorldLoader
{
  public:
    WorldLoader( const ZString &filename );
    ~WorldLoader();

    /// returns true if file was loaded in constructor
    bool isValid() const;

    /// provide GameWorld object to load file into
    void setWorld( GameWorld *target );

    /// Does the deed of loading the world file
    bool go();

    /// Convienience function to return a readied GameWord object
    static GameWorld * loadWorld( const ZString &filename );

  private:
    WorldLoaderPrivate *d;

    /// disabled copy constructor
    WorldLoader(const WorldLoader &);
    /// disabled assignment operator
    WorldLoader & operator=(const WorldLoader &);
};

#endif // WORLD_LOADER_H

