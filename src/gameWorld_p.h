// Insert copyright and license information here.

#ifndef GAME_WORLD_PRIVATE_H
#define GAME_WORLD_PRIVATE_H

typedef std::map<int, GameBoard*> GameBoardMap;

#include "entityManager.h"

class GameWorld;
class GameWorldPrivate
{
  public:
    GameWorldPrivate( GameWorld *pSelf );
    virtual ~GameWorldPrivate();

    EntityManager entityManager;

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

  private:
    GameWorld *self;
};

#endif // GAME_WORLD_PRIVATE_H


