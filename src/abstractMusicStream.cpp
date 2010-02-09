/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include "debug.h"
#include "abstractMusicStream.h"

void AbstractMusicStream::playMusic( const char *song, PriorityType priority )
{
  zdebug() << "Played Song:" << song << "Priority:" << priority;

  begin();

  if ( !hasNotes() || priority > m_priority )
  {
    clear();
    m_priority = priority;
  }

  end();
}

void AbstractMusicStream::playEvent( EventType event )
{
  const char *song = 0;
  PriorityType priority = None;

  switch ( event )
  {
    case Ricochet:
      song = "t9";
      priority = Medium;
      break;

    case Transport:
      song = "ta"; // TODO
      priority = Medium;
      break;

    case Forest:
      song = "ta";
      priority = Medium;
      break;

    case LockedDoor:
      song = "t--gc";
      priority = Medium;
      break;

    case Water:
      song = "t+c+c";
      priority = Medium;
      break;

    case EnergizerOn:
      song = "s.-cd#e";
      priority = Medium;
      break;

    case EnergizerSong:
      song = "s.-f+f-fd#c+c-de";
      priority = Medium;
      break;

    case EnergizerEnd:
      song = "s.-c-a#gf#fd#c";
      priority = Medium;
      break;

    case Key:
      song = "t+cegcegceg+sc";
      priority = Medium;
      break;

    case Door:
      song = "tcgbcgb+ic";
      priority = Medium;
      break;

    case Gem:
      song = "t+c-gec";
      priority = Medium;
      break;

    case Ammo:
      song = "tcc#d";
      priority = Medium;
      break;

    case Torch:
      song = "tcase";
      priority = Medium;
      break;

    case Shoot:
      song = "t+c-c-c";
      priority = Medium;
      break;

    case Ouch:
      song = "t--ct+cd#";
      priority = Medium;
      break;

    case Passage:
      song = "tcegc#fg#df#ad#ga#eg#+c";
      priority = High;
      break;

    case Push:
      song = "t--f";
      priority = Medium;
      break;

    case Breakable:
      song = "t-c";
      priority = Medium;
      break;

    case KillEnemy:
      song = "";
      priority = Medium;
      break;

    case TorchExtinguish:
      song = "";
      priority = Medium;
      break;

    case InvisibleWall:
      song = "t--dc";
      priority = Medium;
      break;

    case Scroll:
      song = "tc-c+d-d+e-e+f-f+g-g";
      priority = Medium;
      break;

    case Duplicator:
      song = "scdefg";
      priority = Medium;
      break;

    case BombSet:
      song = "tcf+cf+c";
      priority = Medium;
      break;

    case BombTick:
      song = "t5";
      priority = Medium;
      break;

    case BombTock:
      song = "t8";
      priority = Medium;
      break;

    case BombBoom:
      song = "ta"; // TODO
      priority = Medium;
      break;

    case FootStep:
      song = "ta"; // TODO
      priority = Low;
      break;

    case ParseError:
      song = "ta"; // TODO
      priority = Highest;
      break;

    default: break;
  }

  if ( song && priority > None ) {
    playMusic( song, priority );
  }
}

