// Insert copyright and license information here.

#include "debug.h"
#include "zztEntity.h"

ZZTEntity ZZTEntity::createEntity( unsigned char id, unsigned char color )
{
  switch (id) {
    case EmptySpace: return ZZTEntity( id, color, 0x00 );
    case EdgeOfBoard: return ZZTEntity( id, color, 0x00 );
    case Player: return ZZTEntity( id, color, 0x02 );
    case Ammo: return ZZTEntity( id, color, 0x84 );
    case Torch: return ZZTEntity( id, color, 0x9d );
    case Gem: return ZZTEntity( id, color, 0x04 );
    case Key: return ZZTEntity( id, color, 0x0c );
    case Door: return ZZTEntity( id, color, 0x0a );
    case Scroll: return ZZTEntity( id, color, 0xe8 );
    case Passage: return ZZTEntity( id, color, 0xf0 );
    case Duplicator: return ZZTEntity( id, color, 0x00 );
    case Bomb: return ZZTEntity( id, color, 0x0b );
    case Energizer: return ZZTEntity( id, color, 0x7f );
    case Star: return ZZTEntity( id, color, 0x0f );
    case ClockwiseConveyer: return ZZTEntity( id, color, 0x00 );
    case CounterclockwiseConveyor: return ZZTEntity( id, color, 0x00 );
    case Bullet: return ZZTEntity( id, color, 0xf8 );
    case Water: return ZZTEntity( id, color, 0xb0 );
    case Forest: return ZZTEntity( id, color, 0xb0 );
    case Solid: return ZZTEntity( id, color, 0xdb );
    case Normal: return ZZTEntity( id, color, 0xb2 );
    case Breakable: return ZZTEntity( id, color, 0xb1 );
    case Boulder: return ZZTEntity( id, color, 0xff );
    case SliderNorthSouth: return ZZTEntity( id, color, 0x12 );
    case SliderEastWest: return ZZTEntity( id, color, 0x1d );
    case Fake: return ZZTEntity( id, color, 0xb2 );
    case InvisibleWall: return ZZTEntity( id, color, 0x00 );
    case BlinkWall: return ZZTEntity( id, color, 0x00 );
    case Transporter: return ZZTEntity( id, color, 0x00 );
    case Line: return ZZTEntity( id, color, 0x00 );
    case Ricochet: return ZZTEntity( id, color, 0x2a );
    case HorizontalBlinkWallRay: return ZZTEntity( id, color, 0xcd );
    case Bear: return ZZTEntity( id, color, 0x99 );
    case Ruffian: return ZZTEntity( id, color, 0x05 );
    case Object: return ZZTEntity( id, color, 0x02 );
    case Slime: return ZZTEntity( id, color, 0x2a );
    case Shark: return ZZTEntity( id, color, 0x5e );
    case SpinningGun: return ZZTEntity( id, color, 0x00 );
    case Pusher: return ZZTEntity( id, color, 0x00 );
    case Lion: return ZZTEntity( id, color, 0xea );
    case Tiger: return ZZTEntity( id, color, 0xe3 );
    case VerticalBlinkWallRay: return ZZTEntity( id, color, 0xba );
    case CentipedeHead: return ZZTEntity( id, color, 0xe9 );
    case CentipedeSegment: return ZZTEntity( id, color, 0x4f );

    // Text entities encode character in color byte
    case BlueText: return ZZTEntity( id, 0x1f, color );
    case GreenText: return ZZTEntity( id, 0x2f, color );
    case CyanText: return ZZTEntity( id, 0x3f, color );
    case RedText: return ZZTEntity( id, 0x4f, color );
    case PurpleText: return ZZTEntity( id, 0x5f, color );
    case YellowText: return ZZTEntity( id, 0x6f, color );
    case WhiteText: return ZZTEntity( id, 0x0f, color );
    case WhiteBlinkingText: return ZZTEntity( id, 0x0f, color );
    case BlueBlinkingText: return ZZTEntity( id, 0x9f, color );
    case GreenBlinkingText: return ZZTEntity( id, 0xaf, color );
    case CyanBlinkingText: return ZZTEntity( id, 0xbf, color );
    case RedBlinkingText: return ZZTEntity( id, 0xcf, color );
    case PurpleBlinkingText: return ZZTEntity( id, 0xdf, color );
    case YellowBlinkingText: return ZZTEntity( id, 0xef, color );
    case GreyBlinkingText: return ZZTEntity( id, 0x07, color );
    default: break;
  }
  return ZZTEntity();
}

static ZZTEntity g_sharedEdgeOfBoardEntity =
    ZZTEntity::createEntity( ZZTEntity::EdgeOfBoard, 0x11 );

const ZZTEntity & ZZTEntity::sharedEdgeOfBoardEntity()
{
  return g_sharedEdgeOfBoardEntity;
}
