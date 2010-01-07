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
};

// -------------------------------------

/// The interactive object superclass
class AbstractThing
{
  public:
    AbstractThing() {/* */};
    virtual ~AbstractThing() {/* */};

    /// set world
    void setWorld( GameWorld *pWorld ) { m_world = pWorld; };
    /// accessor
    GameWorld * world() const { return m_world; };

    /// set board
    void setBoard( GameBoard *pBoard ) { m_board = pBoard; };
    /// accessor
    GameBoard * board() const { return m_board; };

    /// position accessor
    int xPos() const { return position_x; };
    /// position accessor
    int yPos() const { return position_y; };

    /// set X position
    void setXPos( int x ) { position_x = x; };
    /// set Y position
    void setYPos( int y ) { position_y = y; };

    /// set position
    void setPos( int x, int y ) { setXPos(x); setYPos(y); };

    /// set under stuff
    void setUnderEntity( const ZZTEntity &entity ) { under_entity = entity; };
    /// accessor
    const ZZTEntity & underEntity() const { return under_entity; };

    /// set run cycle
    void setCycle( int cycle ) { m_cycle = cycle; };
    /// accessor
    int cycle() const { return m_cycle; };

    /// adjusts the board entity to look like the this thing.
    void updateEntity();

    // accessor
    bool canExec() const { return m_canExec; };

    /// runner, Template Method Pattern.
    void exec();

  protected:
    /// test if movement to a particular space is possible
    bool blockedAt( int x, int y ) const;

    /// move in a direction
    void doMove( int x_step, int y_step );

    /// move in a direction
    void doMove( int direction );

    /// translates a direction into one of the cardinals
    int translateDir( int dir );

    /// random direction of the player
    int seekDir();

    /// random direction, could bump into walls.
    int randAnyDir();

    /// runner, Template Method Pattern.
    virtual void exec_impl() = 0;

  public:
    /// accessor
    virtual unsigned char entityID() const = 0;

    /// accessor
    virtual unsigned char tile() const = 0;

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

class Player : public AbstractThing
{
  public:
    Player();
    virtual unsigned char entityID() const { return ZZTEntity::Player; };
    virtual unsigned char tile() const { return 0x02; };
  protected:
    virtual void exec_impl();
};

// -------------------------------------

class Scroll : public AbstractThing
{
  public:
    Scroll() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Scroll; };
    virtual unsigned char tile() const { return 0xE8; };
  protected:
    virtual void exec_impl() { /* */ };
};

// -------------------------------------

class Passage : public AbstractThing
{
  public:
    Passage() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Passage; };
    virtual unsigned char tile() const { return 0xF0; };
  protected:
    virtual void exec_impl() { /* */ };
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

class Object : public AbstractThing
{
  public:
    Object() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Object; };
    virtual unsigned char tile() const { return 0x01; };
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
  protected:
    virtual void exec_impl() { /* */ };
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

