/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <cstdlib>

#include "debug.h"
#include "zztEntity.h"
#include "gameWorld.h"
#include "gameBoard.h"

#include "zztThing.h"
#include "enemies.h"

using namespace ZZTThing;

// -------------------------------------

Bear::Bear()
  : m_paramSensativity(0)
{
  /* */
}

void Bear::exec_impl()
{
  /* */
}

// -------------------------------------

Ruffian::Ruffian()
  : m_paramIntel(0),
    m_paramRest(0),
    m_moves(0),
    m_rests(0)
{
  /* */
}

void Ruffian::exec_impl()
{
  if ( m_rests == 0 ) {
    m_moves = rand() % 10;
    m_rests = m_paramRest;
    if ( rand() % 9 < m_paramIntel ) {
      m_direction = seekDir();
    }
    else {
      m_direction = randAnyDir();
    }
  }

  if ( m_moves > 0 ) {
    doMove( m_direction );
    m_moves -= 1;
  }
  else if ( m_rests > 0 ) {
    m_rests -= 1;
  }
}

// -------------------------------------

Slime::Slime()
{
  /* */
}

void Slime::exec_impl()
{
  /* */
}

// -------------------------------------

Shark::Shark()
  : m_paramIntel(0)
{
  /* */
}

void Shark::exec_impl()
{
  /* */
}

// -------------------------------------

Lion::Lion()
  : m_paramIntel(0)
{
  /* */
}

void Lion::exec_impl()
{
  doMove( randAnyDir() );
}

// -------------------------------------

Tiger::Tiger()
  : m_paramIntel(0)
{
  /* */
}

void Tiger::exec_impl()
{
  doMove( randAnyDir() );
}

// -------------------------------------

CentipedeHead::CentipedeHead()
  : m_paramIntel(0)
{
  /* */
}

void CentipedeHead::exec_impl()
{
  /* */
}

// -------------------------------------

CentipedeSegment::CentipedeSegment()
{
  /* */
}

void CentipedeSegment::exec_impl()
{
  /* */
}


