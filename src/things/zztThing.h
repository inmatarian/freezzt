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

    /// accessor
    int xPos() const { return position_x; };
    /// accessor
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

    /// adjusts the board entity to look like the this thing.
    void updateEntity();

  protected:
    /// test if movement to a particular space is possible
    bool blockedAt( int x, int y ) const;

    /// move in a direction
    void doMove( int x_step, int y_step );

    /// move in any random direction
    void doRandomAnyMove();

  public:
    /// accessor
    virtual unsigned char entityID() const = 0;

    /// accessor
    virtual unsigned char tile() const = 0;

    /// runner
    virtual void exec() = 0;

  private:
    GameWorld * m_world;
    GameBoard * m_board;

    int position_x;
    int position_y;

    ZZTEntity under_entity;
};

// -------------------------------------

class Player : public AbstractThing
{
  public:
    Player() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Player; };
    virtual unsigned char tile() const { return 0x02; };
    virtual void exec() { /* */ };
};

// -------------------------------------

class Scroll : public AbstractThing
{
  public:
    Scroll() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Scroll; };
    virtual unsigned char tile() const { return 0xE8; };
    virtual void exec() { /* */ };
};

// -------------------------------------

class Passage : public AbstractThing
{
  public:
    Passage() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Passage; };
    virtual unsigned char tile() const { return 0xF0; };
    virtual void exec() { /* */ };
};

// -------------------------------------

class Duplicator : public AbstractThing
{
  public:
    Duplicator() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Duplicator; };
    virtual unsigned char tile() const { return 0x02; };
    virtual void exec() { /* */ };
};

// -------------------------------------

class Bear : public AbstractThing
{
  public:
    Bear() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Bear; };
    virtual unsigned char tile() const { return 0x99; };
    virtual void exec() { /* */ };
};

// -------------------------------------

class Ruffian : public AbstractThing
{
  public:
    Ruffian() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Ruffian; };
    virtual unsigned char tile() const { return 0x05; };
    virtual void exec();
};

// -------------------------------------

class Object : public AbstractThing
{
  public:
    Object() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Object; };
    virtual unsigned char tile() const { return 0x01; };
    virtual void exec() { /* */ };
};

// -------------------------------------

class Slime : public AbstractThing
{
  public:
    Slime() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Slime; };
    virtual unsigned char tile() const { return 0x2A; };
    virtual void exec() { /* */ };
};

// -------------------------------------

class Shark : public AbstractThing
{
  public:
    Shark() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Shark; };
    virtual unsigned char tile() const { return 0x5E; };
    virtual void exec() { /* */ };
};

// -------------------------------------

class SpinningGun : public AbstractThing
{
  public:
    SpinningGun() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::SpinningGun; };
    virtual unsigned char tile() const { return 0x02; };
    virtual void exec() { /* */ };
};

// -------------------------------------

class Pusher : public AbstractThing
{
  public:
    Pusher() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Pusher; };
    virtual unsigned char tile() const { return 0x02; };
    virtual void exec() { /* */ };
};

// -------------------------------------

class Lion : public AbstractThing
{
  public:
    Lion() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Lion; };
    virtual unsigned char tile() const { return 0xEA; };
    virtual void exec() { /* */ };
};

// -------------------------------------

class Tiger : public AbstractThing
{
  public:
    Tiger() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Tiger; };
    virtual unsigned char tile() const { return 0xE3; };
    virtual void exec() { /* */ };
};

// -------------------------------------

class CentipedeHead : public AbstractThing
{
  public:
    CentipedeHead() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::CentipedeHead; };
    virtual unsigned char tile() const { return 0xE9; };
    virtual void exec() { /* */ };
};

// -------------------------------------

class CentipedeSegment : public AbstractThing
{
  public:
    CentipedeSegment() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::CentipedeSegment; };
    virtual unsigned char tile() const { return 0x4F; };
    virtual void exec() { /* */ };
};

// -------------------------------------

class BlinkWall : public AbstractThing
{
  public:
    BlinkWall() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::BlinkWall; };
    virtual unsigned char tile() const { return 0x02; };
    virtual void exec() { /* */ };
};

// -------------------------------------

class Transporter : public AbstractThing
{
  public:
    Transporter() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Transporter; };
    virtual unsigned char tile() const { return 0x02; };
    virtual void exec() { /* */ };
};

// -------------------------------------

class Bullet : public AbstractThing
{
  public:
    Bullet() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Bullet; };
    virtual unsigned char tile() const { return 0xF8; };
    virtual void exec() { /* */ };
};

// -------------------------------------

class Star : public AbstractThing
{
  public:
    Star() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Star; };
    virtual unsigned char tile() const { return 0x02; };
    virtual void exec() { /* */ };
};

// =================

} // namespace

// =================

#endif // ZZT_THING_H

