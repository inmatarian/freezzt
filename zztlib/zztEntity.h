/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef ZZTENTITY_H
#define ZZTENTITY_H

class AbstractPainter;
namespace ZZTThing {
  class AbstractThing;
}

/// floor, wall, or interactive object entities appear on a board's field.
/// should be completely safe for stack creation and copy-by-value usage.
class ZZTEntity
{
  public:
    ZZTEntity()
      : m_id(0), m_color(0x07), m_tile(0), m_thing(0)
    { /* */ }

    ZZTEntity( unsigned char id, unsigned char color, unsigned char tile )
      : m_id(id), m_color(color), m_tile(tile), m_thing(0)
    { /* */ }

    /// accessor
    unsigned char id() const { return m_id; };
    /// sets the entity's id
    void setID( unsigned char id ) { m_id = id; };

    /// accessor
    unsigned char tile() const { return m_tile; };
    /// sets the entity's tile, from the 256 tile bank
    void setTile( unsigned char tile ) { m_tile = tile; };

    /// accessor
    unsigned char color() const { return m_color; };
    /// sets the entity's encoded color, from the 256 back and fore colors.
    void setColor( unsigned char color ) { m_color = color; };

    /// accessor
    ZZTThing::AbstractThing *thing() const { return m_thing; };
    /// sets the entity's thing. Should only carry the pointer.
    void setThing( ZZTThing::AbstractThing *thing ) { m_thing = thing; };

    /// information about an entity
    bool isWalkable() const;
    /// information about an entity
    bool isSwimable() const;
    /// information about an entity
    bool isBreakable() const;
    /// information about an entity
    bool isPushable() const;
    /// information about an entity
    bool isPushable( int x_step, int y_step ) const;
    /// information about an entity
    bool isBoardEdge() const;
    /// information about an entity
    bool isVisibleInDarkness() const;
    /// information about an entity
    bool isThing() const { return (m_thing!=0); };

    /// draws the entity onto the screen at a given position
    void paint( AbstractPainter *painter, int x, int y );

  private:
    unsigned char m_id;
    unsigned char m_color;
    unsigned char m_tile;

    ZZTThing::AbstractThing *m_thing;

  public:
    /// convienience function for creating an entity
    static ZZTEntity createEntity( unsigned char id, unsigned char color );

    /// the edge of board is a special default entity
    static const ZZTEntity &sharedEdgeOfBoardEntity();

    /// list of entities according to ID number.
    enum EntityType
    {
      EmptySpace = 0x00,
      EdgeOfBoard = 0x01,
      Player = 0x04,
      Ammo = 0x05,
      Torch = 0x06,
      Gem = 0x07,
      Key = 0x08,
      Door = 0x09,
      Scroll = 0x0a,
      Passage = 0x0b,
      Duplicator = 0x0c,
      Bomb = 0x0d,
      Energizer = 0x0e,
      Star = 0x0f,
      ClockwiseConveyer = 0x10,
      CounterclockwiseConveyor = 0x11,
      Bullet = 0x12,
      Water = 0x13,
      Forest = 0x14,
      Solid = 0x15,
      Normal = 0x16,
      Breakable = 0x17,
      Boulder = 0x18,
      SliderNorthSouth = 0x19,
      SliderEastWest = 0x1a,
      Fake = 0x1b,
      InvisibleWall = 0x1c,
      BlinkWall = 0x1d,
      Transporter = 0x1e,
      Line = 0x1f,
      Ricochet = 0x20,
      HorizontalBlinkWallRay = 0x21,
      Bear = 0x22,
      Ruffian = 0x23,
      Object = 0x24,
      Slime = 0x25,
      Shark = 0x26,
      SpinningGun = 0x27,
      Pusher = 0x28,
      Lion = 0x29,
      Tiger = 0x2a,
      VerticalBlinkWallRay = 0x2b,
      CentipedeHead = 0x2c,
      CentipedeSegment = 0x2d,
      BlueText = 0x2f,
      GreenText = 0x30,
      CyanText = 0x31,
      RedText = 0x32,
      PurpleText = 0x33,
      YellowText = 0x34,
      WhiteText = 0x35,
      WhiteBlinkingText = 0x36,
      BlueBlinkingText = 0x37,
      GreenBlinkingText = 0x38,
      CyanBlinkingText = 0x39,
      RedBlinkingText = 0x3a,
      PurpleBlinkingText = 0x3b,
      YellowBlinkingText = 0x3c,
      GreyBlinkingText = 0x3d
    };
};

#endif // ZZTENTITY_H

