/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef GAME_BOARD_H
#define GAME_BOARD_H

class GameWorld;
class AbstractPainter;
class ZZTEntity;
class AbstractMusicStream;

namespace ZZTThing {
  class AbstractThing;
  class Player;
  class ProgramBank;
}

class GameBoardPrivate;

/// A single board in a zzt world
class GameBoard
{
  public:
    GameBoard();
    virtual ~GameBoard();

    /// set the currently associated world
    void setWorld( GameWorld *world );
    /// accessor for the currently associated world
    GameWorld *world() const;
    /// convienience accessor
    AbstractMusicStream *musicStream() const;

    /// erases everything on the board
    void clear();

    /// runs one cycle of the board
    void exec();

    /// draws the contents of the board onto the screen
    void paint( AbstractPainter *painter );

    /// accessor for one of the entities on the 60x25 grid
    const ZZTEntity & entity( int x, int y ) const;
    /// adds an entity to the 60x25 grid
    void setEntity( int x, int y, const ZZTEntity &entity );
    /// replaces an entity, deletes associated Thing.
    void replaceEntity( int x, int y, const ZZTEntity &newEntity );
    /// resets an entity to EmptySpace, deletes associated Thing.
    void clearEntity( int x, int y );

    /// accessor for the currently flashing message
    const ZString &message() const;
    /// changes the currently flashing message
    void setMessage( const ZString &mesg );

    /// adds a object that will interact with the board
    void addThing( ZZTThing::AbstractThing *thing );
    /// adds a ProgramBank to the Board's ownership.
    void addProgramBank( ZZTThing::ProgramBank *prog );

    /// move a thing, involves synching things with entities
    void moveThing( ZZTThing::AbstractThing *thing, int newX, int newY );
    /// switches two things
    void switchThings( ZZTThing::AbstractThing *left, ZZTThing::AbstractThing *right );
    /// make a bullet thing
    void makeBullet( int x, int y, int x_step, int y_step, bool playerType );
    /// handler for bullet collisions
    void handleBulletCollision( int x, int y, int x_step, int y_step, bool playerType );
    /// make a star thing
    void makeStar( int x, int y );
    /// removes a thing, sets the ent to whats under
    void deleteThing( ZZTThing::AbstractThing *thing );

    /// sends a message to all appropriately named objects
    void sendLabel( const ZString &to, const ZString &label,
                    const ZZTThing::AbstractThing *from );

    /// Returns the player for the board. Every board must have one.
    ZZTThing::Player *player() const;

    /// accessor to the northern exit
    int northExit() const;
    /// accessor to the southern exit
    int southExit() const;
    /// accessor to the western exit
    int westExit() const;
    /// accessor to the eastern exit
    int eastExit() const;

    /// accessor
    bool isDark() const;
    /// sets the Darkness Bit on a board
    void setDark( bool dark );

    /// set the northern exit
    void setNorthExit( int exit );
    /// set to the southern exit
    void setSouthExit( int exit );
    /// set to the western exit
    void setWestExit( int exit );
    /// set to the eastern exit
    void setEastExit( int exit );

    /// accessor to the board cycle for Thing timing
    unsigned int cycle() const;

    /// move pushable entities in a given direction, starting with a pushable
    void pushEntities( int x, int y, int x_step, int y_step );

  private:
    GameBoardPrivate *d;
};

#endif // GAME_BOARD_H


