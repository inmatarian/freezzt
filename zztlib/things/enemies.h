/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef ENEMIES_H
#define ENEMIES_H

#include <list>

#include "zztEntity.h"
#include "zztThing.h"

// =================

namespace ZZTThing {

// =================

class Bear : public AbstractThing
{
  public:
    Bear();
    virtual unsigned char entityID() const { return ZZTEntity::Bear; };
    virtual unsigned char tile() const { return 0x99; };

    void setSensativity( int sense ) { m_paramSensativity = sense; };

  protected:
    virtual void exec_impl();

  private:
    int m_paramSensativity;
};

// -------------------------------------

class Ruffian : public AbstractThing
{
  public:
    Ruffian();
    virtual unsigned char entityID() const { return ZZTEntity::Ruffian; };
    virtual unsigned char tile() const { return 0x05; };

    void setIntelligence( int intel ) { m_paramIntel = intel; };
    void setRest( int rest ) { m_paramRest = rest; };

  protected:
    virtual void exec_impl();

  private:
    int m_paramIntel;
    int m_paramRest;
    int m_moves;
    int m_rests;
    int m_direction;
};

// -------------------------------------

class Slime : public AbstractThing
{
  public:
    Slime();
    virtual unsigned char entityID() const { return ZZTEntity::Slime; };
    virtual unsigned char tile() const { return 0x2A; };

    void setSpeed( int speed ) { m_paramSpeed = speed; };

  protected:
    virtual void exec_impl();

  private:
    int m_paramSpeed;
};

// -------------------------------------

class Shark : public AbstractThing
{
  public:
    Shark();
    virtual unsigned char entityID() const { return ZZTEntity::Shark; };
    virtual unsigned char tile() const { return 0x5E; };

    void setIntelligence( int intel ) { m_paramIntel = intel; };

  protected:
    virtual void exec_impl();

  private:
    int m_paramIntel;
};

// -------------------------------------

class Lion : public AbstractThing
{
  public:
    Lion();
    virtual unsigned char entityID() const { return ZZTEntity::Lion; };
    virtual unsigned char tile() const { return 0xEA; };

    void setIntelligence( int intel ) { m_paramIntel = intel; };

  protected:
    virtual void exec_impl();

  private:
    int m_paramIntel;
};

// -------------------------------------

class Tiger : public AbstractThing
{
  public:
    Tiger();
    virtual unsigned char entityID() const { return ZZTEntity::Tiger; };
    virtual unsigned char tile() const { return 0xE3; };

    void setIntelligence( int intel ) { m_paramIntel = intel; };

  protected:
    virtual void exec_impl();

  private:
    int m_paramIntel;
};

// -------------------------------------

class CentipedeSegment;
typedef std::list<CentipedeSegment*> CentipedeBody;

class CentipedeHead : public AbstractThing
{
  public:
    CentipedeHead();
    virtual unsigned char entityID() const { return ZZTEntity::CentipedeHead; };
    virtual unsigned char tile() const { return 0xE9; };

    void setIntelligence( int intel ) { m_paramIntel = intel; };
    void setDeviance( int deviance ) { m_paramDeviance = deviance; };

    void moveSegments( int oldX, int oldY );
    void findSegments();
    void switchHeadAndTail();

    virtual void handleDeleted();
    void handleLosingSegment( CentipedeSegment *segment );

  protected:
    virtual void exec_impl();

  private:
    CentipedeBody m_body;
    int m_paramIntel;
    int m_paramDeviance;
    int m_direction;
    int m_settleTime;
};

// -------------------------------------

class CentipedeSegment : public AbstractThing
{
  public:
    CentipedeSegment();
    virtual unsigned char entityID() const { return ZZTEntity::CentipedeSegment; };
    virtual unsigned char tile() const { return 0x4F; };

    CentipedeHead *head() const { return m_head; };
    void setHead( CentipedeHead *head ) { m_head = head; };

    void becomeHead();
    virtual void handleDeleted();

  protected:
    virtual void exec_impl();

  private:
    CentipedeHead *m_head;
    int m_isolated;
};

// =================

} // namespace

// =================

#endif // ENEMIES_H


