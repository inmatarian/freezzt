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
class ZZTThing;
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
    void addThing( ZZTThing *thing );

    /// accessor to the northern exit
    int northExit() const;

    /// accessor to the southern exit
    int southExit() const;

    /// accessor to the western exit
    int westExit() const;

    /// accessor to the eastern exit
    int eastExit() const;

    /// accessor to the northern exit
    void setNorthExit( int exit );

    /// accessor to the southern exit
    void setSouthExit( int exit );

    /// accessor to the western exit
    void setWestExit( int exit );

    /// accessor to the eastern exit
    void setEastExit( int exit );

  private:
    GameBoardPrivate *d;
};

#endif // GAME_BOARD_H


