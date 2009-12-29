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

    /// adjusts the board entity to look like the this thing.
    virtual void updateEntity();

    /// accessor
    virtual unsigned char entityID() const = 0;

    /// accessor
    virtual unsigned char tile() const = 0;

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

  private:
    GameWorld * m_world;
    GameBoard * m_board;

    int position_x;
    int position_y;
};

// -------------------------------------

class Player : public AbstractThing
{
  public:
    Player() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Player; };
    virtual unsigned char tile() const { return 0x02; };
};

// -------------------------------------

class Scroll : public AbstractThing
{
  public:
    Scroll() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Scroll; };
    virtual unsigned char tile() const { return 0xE8; };
};

// -------------------------------------

class Passage : public AbstractThing
{
  public:
    Passage() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Passage; };
    virtual unsigned char tile() const { return 0xF0; };
};

// -------------------------------------

class Duplicator : public AbstractThing
{
  public:
    Duplicator() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Duplicator; };
    virtual unsigned char tile() const { return 0x02; };
};

// -------------------------------------

class Bear : public AbstractThing
{
  public:
    Bear() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Bear; };
    virtual unsigned char tile() const { return 0x99; };
};

// -------------------------------------

class Object : public AbstractThing
{
  public:
    Object() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Object; };
    virtual unsigned char tile() const { return 0x01; };
};

// -------------------------------------

class Slime : public AbstractThing
{
  public:
    Slime() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Slime; };
    virtual unsigned char tile() const { return 0x2A; };
};

// -------------------------------------

class Shark : public AbstractThing
{
  public:
    Shark() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Shark; };
    virtual unsigned char tile() const { return 0x5E; };
};

// -------------------------------------

class SpinningGun : public AbstractThing
{
  public:
    SpinningGun() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::SpinningGun; };
    virtual unsigned char tile() const { return 0x02; };
};

// -------------------------------------

class Pusher : public AbstractThing
{
  public:
    Pusher() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Pusher; };
    virtual unsigned char tile() const { return 0x02; };
};

// -------------------------------------

class Lion : public AbstractThing
{
  public:
    Lion() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Lion; };
    virtual unsigned char tile() const { return 0xEA; };
};

// -------------------------------------

class Tiger : public AbstractThing
{
  public:
    Tiger() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Tiger; };
    virtual unsigned char tile() const { return 0xE3; };
};

// -------------------------------------

class CentipedeHead : public AbstractThing
{
  public:
    CentipedeHead() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::CentipedeHead; };
    virtual unsigned char tile() const { return 0xE9; };
};

// -------------------------------------

class CentipedeSegment : public AbstractThing
{
  public:
    CentipedeSegment() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::CentipedeSegment; };
    virtual unsigned char tile() const { return 0x4F; };
};

// -------------------------------------

class BlinkWall : public AbstractThing
{
  public:
    BlinkWall() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::BlinkWall; };
    virtual unsigned char tile() const { return 0x02; };
};

// -------------------------------------

class Transporter : public AbstractThing
{
  public:
    Transporter() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Transporter; };
    virtual unsigned char tile() const { return 0x02; };
};

// -------------------------------------

class Bullet : public AbstractThing
{
  public:
    Bullet() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Bullet; };
    virtual unsigned char tile() const { return 0xF8; };
};

// -------------------------------------

class Star : public AbstractThing
{
  public:
    Star() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Star; };
    virtual unsigned char tile() const { return 0x02; };
};

// =================

} // namespace

// =================

#endif // ZZT_THING_H

