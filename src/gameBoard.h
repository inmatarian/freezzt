// Insert copyright and license information here.

#ifndef GAME_BOARD_H
#define GAME_BOARD_H

class GameWorld;

class GameBoardPrivate;
class GameBoard
{
  public:
    GameBoard();
    virtual ~GameBoard();

    void setWorld( GameWorld *world );
    GameWorld *world() const;

  private:
    GameBoardPrivate *d;
};

#endif // GAME_BOARD_H


