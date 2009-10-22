// Insert copyright and license information here.

#ifndef GAME_WORLD_H
#define GAME_WORLD_H

#include "observer/abstractNotifier.h"

class GameBoard;
class AbstractPainter;
class EntityManager;

class GameWorldPrivate;
class GameWorld : public AbstractNotifier
{
  public:
    GameWorld();
    virtual ~GameWorld();

    enum Signals {
      no_signal = 0,
      AMMO_CHANGED,
      GEMS_CHANGED,
      HEALTH_CHANGED,
      TORCHES_CHANGED,
      TORCH_CYCLES_CHANGED,
      SCORE_CHANGED,
      ENERGIZER_CYCLES_CHANGED,
      TIME_PASSED_CHANGED,
      KEYS_CHANGED,
      FLAGS_CHANGED,
      CURRENT_BOARD_CHANGED,
      max_signal
    };

    EntityManager * entityManager() const;

    void addBoard( int index, GameBoard *board );
    GameBoard *getBoard( int index ) const;
    int indexOf( GameBoard *board ) const;

    void setCurrentBoard( GameBoard *board );
    GameBoard *currentBoard() const;

    void setCurrentAmmo( int ammo );
    int currentAmmo() const;

    void setCurrentGems( int gems );
    int currentGems() const;
    
    void setCurrentHealth( int hp );
    int currentHealth() const;

    void setCurrentTorches( int torch );
    int currentTorches() const;

    void setCurrentTorchCycles( int cycles );
    int currentTorchCycles() const;

    void setCurrentScore( int score );
    int currentScore() const;

    void setCurrentEnergizerCycles( int cycles );
    int currentEnergizerCycles() const;

    void setCurrentTimePassed( int time );
    int currentTimePassed() const;

    enum DoorKeys {
      no_doorkey = -1,
      BLUE_DOORKEY,
      GREEN_DOORKEY,
      CYAN_DOORKEY,
      RED_DOORKEY,
      PURPLE_DOORKEY,
      YELLOW_DOORKEY,
      WHITE_DOORKEY,
      max_doorkey
    };

    void addDoorKey( int keyType );
    void removeDoorKey( int keyType );
    bool hasDoorKey( int keyType );

    void setWorldTitle( const std::string &title );
    const std::string &worldTitle() const;

    void addGameFlag( const std::string &flag );
    void removeGameFlag( const std::string &flag );
    bool hasGameFlag( const std::string &flag );

    void paint( AbstractPainter *painter );

  private:
    GameWorldPrivate *d;
};

#endif // GAME_WORLD_H


