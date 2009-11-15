// Insert copyright and license information here.

#ifndef GAME_BOARD_PRIVATE_H
#define GAME_BOARD_PRIVATE_H

class ZZTEntity;
class ZZTThing;

typedef std::list<ZZTThing*> ThingList;

class GameBoard;
class GameBoardPrivate
{
  public:
    GameBoardPrivate( GameBoard *pSelf );
    virtual ~GameBoardPrivate();

    GameWorld *world;

    ZZTEntity *field;
    ThingList thingList;

    std::string message;
    int northExit;
    int southExit;
    int westExit;
    int eastExit;

  private:
    GameBoard *self;
};

#endif // GAME_BOARD_PRIVATE_H

