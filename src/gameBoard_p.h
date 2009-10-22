// Insert copyright and license information here.

#ifndef GAME_BOARD_PRIVATE_H
#define GAME_BOARD_PRIVATE_H

class GameBoard;
class GameBoardPrivate
{
  public:
    GameBoardPrivate( GameBoard *pSelf );
    virtual ~GameBoardPrivate();

    GameWorld *world;

  private:
    GameBoard *self;
};

#endif // GAME_BOARD_PRIVATE_H

