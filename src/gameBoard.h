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

namespace ZZTThing {
  class AbstractThing;
  class Player;
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

    /// accessor for the currently flashing message
    const std::string &message() const;

    /// changes the currently flashing message
    void setMessage( const std::string &mesg );

    /// adds a object that will interact with the board
    void addThing( ZZTThing::AbstractThing *thing );

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

  private:
    GameBoardPrivate *d;
};

#endif // GAME_BOARD_H


