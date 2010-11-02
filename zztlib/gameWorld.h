/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef GAME_WORLD_H
#define GAME_WORLD_H

class GameBoard;
class AbstractPainter;
class AbstractMusicStream;
class GameWorldPrivate;
class ScrollView;

/// A complete gameworld that can be played.
class GameWorld
{
  public:
    GameWorld();
    virtual ~GameWorld();

    static GameWorld *createEmptyWorld();

    /// adds a board to the world
    void addBoard( int index, GameBoard *board );
    /// accessor to a board from the world
    GameBoard *getBoard( int index ) const;
    /// returns the integer index of a given board in the world
    int indexOf( GameBoard *board ) const;
    /// total number of boards
    int maxBoards() const;

    /// changes board at the start of the next event loop
    void changeActiveBoard( int index );

    /// changes the current board
    void setCurrentBoard( GameBoard *board );
    /// accessor for the current board
    GameBoard *currentBoard() const;

    /// sets the index of the starting board
    void setStartBoard( int index );
    /// accessor of the starting board
    int startBoard() const;

    /// sets the current ammo
    void setCurrentAmmo( int ammo );
    /// accessor
    int currentAmmo() const;

    /// sets the current gem count
    void setCurrentGems( int gems );
    /// accessor
    int currentGems() const;
    
    /// sets the current health
    void setCurrentHealth( int hp );
    /// accessor
    int currentHealth() const;

    /// sets the current torches
    void setCurrentTorches( int torch );
    /// accessor
    int currentTorches() const;

    /// sets the current torch cycle
    void setCurrentTorchCycles( int cycles );
    /// accessor
    int currentTorchCycles() const;

    /// sets the current score
    void setCurrentScore( int score );
    /// accessor
    int currentScore() const;

    /// sets the current energizer life cycles
    void setCurrentEnergizerCycles( int cycles );
    /// accessor
    int currentEnergizerCycles() const;

    /// sets the current amount of time
    void setCurrentTimePassed( int time );
    /// accessor
    int currentTimePassed() const;

    /// list of door keys
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

    /// adds a door key that the player has
    void addDoorKey( int keyType );
    /// removes one of the player's door keys
    void removeDoorKey( int keyType );
    /// accessor
    bool hasDoorKey( int keyType );

    /// sets the world title
    void setWorldTitle( const std::string &title );
    /// accessor
    const std::string &worldTitle() const;

    /// adds a game flag
    void addGameFlag( const std::string &flag );
    /// removes a game flag
    void removeGameFlag( const std::string &flag );
    /// accessor
    bool hasGameFlag( const std::string &flag );

    /// starts an input key press
    void addInputKey( int keycode, int unicode );
    /// clears
    void clearInputKeys();
    /// accessor
    bool upPressed() const;
    /// accessor
    bool downPressed() const;
    /// accessor
    bool leftPressed() const;
    /// accessor
    bool rightPressed() const;
    /// accessor
    bool shootUpPressed() const;
    /// accessor
    bool shootDownPressed() const;
    /// accessor
    bool shootLeftPressed() const;
    /// accessor
    bool shootRightPressed() const;

    /// marks a board-transtion tile on the 60x25 grid on or off
    void setTransitionTile( int x, int y, bool on );
    /// accessor
    bool transitionTile( int x, int y ) const;

    /// based on the frame delay setting, does nothing or runs update
    void update();

    /// runs one cycle of the world
    void exec();

    /// paints the world and current board to the 80x25 grid
    void paint( AbstractPainter *painter );

    /// music stream for playing sound effects
    void setMusicStream( AbstractMusicStream *musicStream );
    /// accessor
    AbstractMusicStream *musicStream() const;

    /// scroll for manipulating
    void setScrollView( ScrollView *view );
    /// accessor
    ScrollView *scrollView() const;

    /// sets the frame delay setting
    void setFrameCycle( int setting );

    /// gets the frame delay setting
    int frameCycle() const;

  private:
    GameWorldPrivate *d;
};

#endif // GAME_WORLD_H


