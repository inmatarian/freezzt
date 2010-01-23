/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef GAME_WORLD_PRIVATE_H
#define GAME_WORLD_PRIVATE_H

#include "defines.h"

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

    // keyboard presses from the player
    bool pressed_up;
    bool pressed_down;
    bool pressed_left;
    bool pressed_right;
    bool pressed_shoot_up;
    bool pressed_shoot_down;
    bool pressed_shoot_left;
    bool pressed_shoot_right;
    bool pressed_torch;

    std::string worldTitle;
    std::list<std::string> gameFlags;

    GameBoardMap boards;
    int maxBoards;
    GameBoard *currentBoard;

    int boardSwitch;

    int transitionCount;
    bool *transitionTiles;

  private:
    GameWorld *self;
};

#endif // GAME_WORLD_PRIVATE_H


