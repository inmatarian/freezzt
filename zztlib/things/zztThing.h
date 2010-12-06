/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef ZZT_THING_H
#define ZZT_THING_H

#include "zztEntity.h"

class GameWorld;
class GameBoard;
class AbstractMusicStream;

// =================

namespace ZZTThing {

// =================

enum Direction {
  Idle = 0,
  North,
  South,
  West,
  East,
  Seek,
  RandAny,
  RandNotBlocked
};

// -------------------------------------

/// The interactive object superclass
class AbstractThing
{
  public:
    AbstractThing() {/* */};
    virtual ~AbstractThing() {/* */};

    /// set board
    void setBoard( GameBoard *pBoard );
    /// accessor
    GameBoard *board() const;
    /// convienience accessor
    GameWorld *world() const;
    /// convienience accessor
    AbstractMusicStream *musicStream() const;

    /// position accessor
    int xPos() const;
    /// position accessor
    int yPos() const;

    /// set X position
    void setXPos( int x );
    /// set Y position
    void setYPos( int y );

    /// set position
    void setPos( int x, int y );

    /// set under stuff
    void setUnderEntity( const ZZTEntity &entity );
    /// accessor
    const ZZTEntity & underEntity() const;

    /// set run cycle
    void setCycle( int cycle );
    /// accessor
    int cycle() const;

    /// adjusts the board entity to look like the this thing.
    void updateEntity();

    /// runner, Template Method Pattern.
    void exec();

    /// handler for when being removed from the board
    virtual void handleDeleted() { /* */ };

    /// handler for when being touched by the player
    virtual void handleTouched() { /* */ };

    /// random direction of the player
    int seekDir();

    /// the direction the object is currently moving
    int flowDir();

  protected:
    /// test if movement to a particular space is possible
    bool blocked( int old_x, int old_y, int x_step, int y_step ) const;

    /// test if movement to a particular space is possible
    bool blocked( int x_step, int y_step ) const;

    /// test if movement to a particular space is possible
    bool blocked( int dir ) const;

    /// move in a direction
    void doMove( int old_x, int old_y, int x_step, int y_step );

    /// move in a direction
    void doMove( int x_step, int y_step );

    /// move in a direction
    void doMove( int direction );

    /// move to a position
    void gotoPos( int x, int y );

    /// translates a direction into one of the cardinals
    int translateDir( int dir );

    /// translates a step pair into a direction;
    int translateStep( int x, int y );

    /// random direction, could bump into walls.
    int randAnyDir();

    /// random direction, only directions safe to move.
    int randNotBlockedDir();

    /// shoot a bullet
    void doShoot( int x_step, int y_step, bool playerType );

    /// shoot a bullet
    void doShoot( int dir, bool playerType );

    /// remove the thing, delete it.
    void doDie();

    /// dispatcher to handlers
    virtual void interact( int old_x, int old_y, int dx, int dy );

    virtual void handleEdgeOfBoard( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handleForest( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handleBreakable( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handlePlayer( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handleAmmo( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handleTorch( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handleGem( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handleKey( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handleDoor( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handleScroll( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handlePassage( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handleBomb( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handleEnergizer( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handleStar( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handleBullet( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handleWater( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handleIvisibleWall( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handleTransporter( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handleHorizontalBlinkWallRay( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handleBear( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handleRuffian( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handleObject( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handleSlime( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handleShark( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handleSpinningGun( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handleLion( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handleTiger( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handleVerticalBlinkWallRay( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handleCentipedeHead( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};
    virtual void handleCentipedeSegment( const ZZTEntity &ent, int ox, int oy, int dx, int dy ) {/* */};

    /// runner, Template Method Pattern.
    virtual void exec_impl() = 0;

  public:
    /// accessor
    virtual unsigned char entityID() const = 0;

    /// accessor
    virtual unsigned char tile() const = 0;

    /// property
    virtual bool pushes() const { return false; };

  private:
    GameWorld * m_world;
    GameBoard * m_board;

    int position_x;
    int position_y;
    int m_cycle;
    bool m_canExec;

    ZZTEntity under_entity;
};

// -------------------------------------

class Passage : public AbstractThing
{
  public:
    Passage() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Passage; };
    virtual unsigned char tile() const { return 0xF0; };

    void setDestination( unsigned char dest ) { m_destination = dest; };
    unsigned char destination() const { return m_destination; };

  protected:
    virtual void exec_impl() { /* */ };

  private:
    unsigned char m_destination;
};

// -------------------------------------

class Duplicator : public AbstractThing
{
  public:
    Duplicator() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Duplicator; };
    virtual unsigned char tile() const { return 0x02; };
  protected:
    virtual void exec_impl() { /* */ };
};

// -------------------------------------

class SpinningGun : public AbstractThing
{
  public:
    SpinningGun() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::SpinningGun; };
    virtual unsigned char tile() const { return 0x02; };
  protected:
    virtual void exec_impl() { /* */ };
};

// -------------------------------------

class Pusher : public AbstractThing
{
  public:
    Pusher() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Pusher; };
    virtual unsigned char tile() const { return 0x02; };
  protected:
    virtual void exec_impl() { /* */ };
};

// -------------------------------------

class BlinkWall : public AbstractThing
{
  public:
    BlinkWall() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::BlinkWall; };
    virtual unsigned char tile() const { return 0x02; };
  protected:
    virtual void exec_impl() { /* */ };
};

// -------------------------------------

class Transporter : public AbstractThing
{
  public:
    Transporter() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Transporter; };
    virtual unsigned char tile() const { return 0x02; };
  protected:
    virtual void exec_impl() { /* */ };
};

// -------------------------------------

class Bullet : public AbstractThing
{
  public:
    Bullet() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Bullet; };
    virtual unsigned char tile() const { return 0xF8; };

    void setDirection( int dir ) { mDirection = dir; };
    void setDirection( int x, int y ) {
      mDirection = translateStep( x, y );
    };

    void setType( bool playerType ) { mPlayerType = playerType; };

  protected:
    virtual void exec_impl();
    virtual void interact( int old_x, int old_y, int dx, int dy );

  private:
    int mDirection;
    bool mPlayerType;
};

// -------------------------------------

class Star : public AbstractThing
{
  public:
    Star() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Star; };
    virtual unsigned char tile() const { return 0x02; };
  protected:
    virtual void exec_impl() { /* */ };
};

// =================

} // namespace

// =================

#endif // ZZT_THING_H

