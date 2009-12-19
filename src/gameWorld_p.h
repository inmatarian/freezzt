// Insert copyright and license information here.

#ifndef GAME_WORLD_PRIVATE_H
#define GAME_WORLD_PRIVATE_H

typedef std::map<int, GameBoard*> GameBoardMap;

class GameWorld;
class GameWorldPrivate
{
  public:
    GameWorldPrivate( GameWorld *pSelf );
    virtual ~GameWorldPrivate();

    int startBoard;
    int currentAmmo;
    int currentGems;
    int currentHealth;
    int currentTorches;
    int currentTorchCycles;
    int currentScore;
    int currentEnergizerCycles;
    int currentTimePassed;
    bool doorKeys[ GameWorld::max_doorkey ];

    std::string worldTitle;
    std::list<std::string> gameFlags;

    GameBoardMap boards;
    int maxBoards;
    GameBoard *currentBoard;

    int transitionCount;
    bool *transitionTiles;

  private:
    GameWorld *self;
};

#endif // GAME_WORLD_PRIVATE_H


