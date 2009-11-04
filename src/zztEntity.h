// Insert copyright and license information here.

#ifndef ZZTENTITY_H
#define ZZTENTITY_H

class ZZTEntity
{
  public:
    ZZTEntity()
      : m_id(0), m_color(0x07), m_tile(0)
    { /* */ }

    ZZTEntity( unsigned char id, unsigned char color, unsigned char tile )
      : m_id(id), m_color(color), m_tile(tile)
    { /* */ }

    unsigned char id() const { return m_id; };
    void setID( unsigned char id ) { m_id = id; };

    unsigned char tile() const { return m_tile; };
    void setTile( unsigned char tile ) { m_tile = tile; };

    unsigned char color() const { return m_color; };
    void setColor( unsigned char color ) { m_color = color; };

  private:
    unsigned char m_id;
    unsigned char m_color;
    unsigned char m_tile;

  public:
    static ZZTEntity createEntity( unsigned char id, unsigned char color );
    static const ZZTEntity &sharedEdgeOfBoardEntity();

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

