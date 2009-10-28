// Insert copyright and license information here.

#ifndef GAME_BOARD_H
#define GAME_BOARD_H

class GameWorld;
class AbstractPainter;
class ZZTEntity;

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

    ZZTEntity *entity( int x, int y ) const;
    void setEntity( int x, int y, ZZTEntity *entity );

  private:
    GameBoardPrivate *d;
};

#endif // GAME_BOARD_H


