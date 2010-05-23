/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <cctype>
#include <cassert>

#include "debug.h"
#include "abstractMusicStream.h"

void AbstractMusicStream::playMusic( const char *song, PriorityType priority )
{
  assert( m_begun );
  // zdebug() << "Played Song:" << song << "Priority:" << priority;

  if ( !hasNotes() ) {
    m_priority = priority;
  }
  else if ( priority > m_priority ) {
    clear();
    m_priority = priority;
  }

  int octave_shift = 0;
  int duration = 1;

  for ( int i = 0; song[i]; i++ )
  {
    const char c = tolower(song[i]);

    if ( c == '+' )      { octave_shift += 12; }
    else if ( c == '-' ) { octave_shift -= 12; }
    else if ( c == '.' ) { duration += duration / 2; }
    else if ( c == '3' ) { duration /= 3; }
    else if ( c >= '0' && c <= '9' )
    {
      addNote( false, c - '0', 1 );
    }
    else if ( c == 'w' ) { duration = 32; }
    else if ( c == 'h' ) { duration = 16; }
    else if ( c == 'q' ) { duration = 8; }
    else if ( c == 'i' ) { duration = 4; }
    else if ( c == 's' ) { duration = 2; }
    else if ( c == 't' ) { duration = 1; }
    else if ( c >= 'a' && c <= 'g' )
    {
      int n;
      if ( c <= 'b' ) { n = 49 + c - 'a'; }
      else            { n = 40 + c - 'c'; }

      if ( song[i+1] == '#' )      { n += 1; }
      else if ( song[i+1] == '!' ) { n -= 1; }

      addNote( true, n + octave_shift, duration );
    }
    else if ( c == 'x' ) { addNote( false, -1, duration ); }
  }
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
      song = "ta"; // TODO
      priority = Medium;
      break;

    case TorchExtinguish:
      song = "ta"; // TODO
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

