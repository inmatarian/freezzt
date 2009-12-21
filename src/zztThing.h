// Insert copyright and license information here.

#ifndef ZZT_THING_H
#define ZZT_THING_H

#include "zztEntity.h"

// correspond to Object superclass
class ZZTThing
{
  public:
    ZZTThing() {/* */};
    virtual ~ZZTThing() {/* */};

    virtual unsigned char entityID() const = 0;
    virtual unsigned char tile() const = 0;

    int xPos() const { return position_x; };
    int yPos() const { return position_y; };
    void setXPos( int x ) { position_x = x; };
    void setYPos( int y ) { position_y = y; };
    void setPos( int x, int y ) { setXPos(x); setYPos(y); };

  private:
    int position_x;
    int position_y;
};

// -------------------------------------

class Player : public ZZTThing
{
  public:
    Player() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Player; };
    virtual unsigned char tile() const { return 0x02; };
};

// -------------------------------------

class Scroll : public ZZTThing
{
  public:
    Scroll() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Scroll; };
    virtual unsigned char tile() const { return 0xE8; };
};

// -------------------------------------

class Passage : public ZZTThing
{
  public:
    Passage() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Passage; };
    virtual unsigned char tile() const { return 0xF0; };
};

// -------------------------------------

class Duplicator : public ZZTThing
{
  public:
    Duplicator() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Duplicator; };
    virtual unsigned char tile() const { return 0x02; };
};

// -------------------------------------

class Bear : public ZZTThing
{
  public:
    Bear() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Bear; };
    virtual unsigned char tile() const { return 0x99; };
};

// -------------------------------------

class Object : public ZZTThing
{
  public:
    Object() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Object; };
    virtual unsigned char tile() const { return 0x01; };
};

// -------------------------------------

class Slime : public ZZTThing
{
  public:
    Slime() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Slime; };
    virtual unsigned char tile() const { return 0x2A; };
};

// -------------------------------------

class Shark : public ZZTThing
{
  public:
    Shark() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Shark; };
    virtual unsigned char tile() const { return 0x5E; };
};

// -------------------------------------

class SpinningGun : public ZZTThing
{
  public:
    SpinningGun() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::SpinningGun; };
    virtual unsigned char tile() const { return 0x02; };
};

// -------------------------------------

class Pusher : public ZZTThing
{
  public:
    Pusher() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Pusher; };
    virtual unsigned char tile() const { return 0x02; };
};

// -------------------------------------

class Lion : public ZZTThing
{
  public:
    Lion() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Lion; };
    virtual unsigned char tile() const { return 0xEA; };
};

// -------------------------------------

class Tiger : public ZZTThing
{
  public:
    Tiger() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Tiger; };
    virtual unsigned char tile() const { return 0xE3; };
};

// -------------------------------------

class CentipedeHead : public ZZTThing
{
  public:
    CentipedeHead() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::CentipedeHead; };
    virtual unsigned char tile() const { return 0xE9; };
};

// -------------------------------------

class CentipedeSegment : public ZZTThing
{
  public:
    CentipedeSegment() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::CentipedeSegment; };
    virtual unsigned char tile() const { return 0x4F; };
};

// -------------------------------------

class BlinkWall : public ZZTThing
{
  public:
    BlinkWall() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::BlinkWall; };
    virtual unsigned char tile() const { return 0x02; };
};

// -------------------------------------

class Transporter : public ZZTThing
{
  public:
    Transporter() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Transporter; };
    virtual unsigned char tile() const { return 0x02; };
};

// -------------------------------------

class Bullet : public ZZTThing
{
  public:
    Bullet() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Bullet; };
    virtual unsigned char tile() const { return 0xF8; };
};

// -------------------------------------

class Star : public ZZTThing
{
  public:
    Star() {/* */};
    virtual unsigned char entityID() const { return ZZTEntity::Star; };
    virtual unsigned char tile() const { return 0x02; };
};

#endif // ZZT_THING_H
