// Insert copyright and license information here.

#ifndef ZZTENTITY_H
#define ZZTENTITY_H

class ZZTEntity
{
  public:
    virtual unsigned char id() const = 0;
    virtual unsigned char tile() const = 0;

    virtual bool isMutable() const = 0;

    unsigned char color() const { return m_color; };
    void setColor( unsigned char c ) { m_color = c; };

  private:
    unsigned char m_color;
};

// ---------------------------------------------------------------------------
// The Immutable and Mutable classes are for the convenience of defining
// the flyweights in the EntityManager.

class ImmutableEntity : public ZZTEntity
{
  public:
    virtual bool isMutable() const { return false; };
};

class MutableEntity : public ZZTEntity
{
  public:
    virtual bool isMutable() const { return true; };
};

// ---------------------------------------------------------------------------

class EmptySpaceEntity : public ImmutableEntity
{
  public:
    virtual unsigned char id() const { return 0; }
    virtual unsigned char tile() const { return ' '; }
};

class ForestEntity : public ImmutableEntity
{
  public:
    virtual unsigned char id() const { return 0x14; }
    virtual unsigned char tile() const { return 0xB0; }
};

class SolidEntity : public ImmutableEntity
{
  public:
    virtual unsigned char id() const { return 0x15; }
    virtual unsigned char tile() const { return 0xDB; }
};

class NormalEntity : public ImmutableEntity
{
  public:
    virtual unsigned char id() const { return 0x16; }
    virtual unsigned char tile() const { return 0xB2; }
};

class BreakableEntity : public ImmutableEntity
{
  public:
    virtual unsigned char id() const { return 0x17; }
    virtual unsigned char tile() const { return 0xB1; }
};

class FakeEntity : public ImmutableEntity
{
  public:
    virtual unsigned char id() const { return 0x1B; }
    virtual unsigned char tile() const { return 0xB2; }
};

#endif // ZZTENTITY_H

