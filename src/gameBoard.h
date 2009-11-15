// Insert copyright and license information here.

#ifndef GAME_BOARD_H
#define GAME_BOARD_H

class GameWorld;
class AbstractPainter;
class ZZTEntity;
class ZZTThing;

class GameBoardPrivate;
class GameBoard
{
  public:
    GameBoard();
    virtual ~GameBoard();

    void setWorld( GameWorld *world );
    GameWorld *world() const;

    void clear();

    void paint( AbstractPainter *painter );

    const ZZTEntity & entity( int x, int y ) const;
    void setEntity( int x, int y, const ZZTEntity &entity );

    const std::string &message() const;
    void setMessage( const std::string &mesg );

    void addThing( ZZTThing *thing );

    int northExit() const;
    int southExit() const;
    int westExit() const;
    int eastExit() const;

    void setNorthExit( int exit );
    void setSouthExit( int exit );
    void setWestExit( int exit );
    void setEastExit( int exit );

  private:
    GameBoardPrivate *d;
};

#endif // GAME_BOARD_H


