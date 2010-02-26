/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <cstdlib>
#include <cassert>
#include <list>
#include <vector>
#include <algorithm>

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

// reference notes:
// when surrounded by too many segments, the head can't decide and the
// segments become heads

CentipedeHead::CentipedeHead()
  : m_paramIntel(0),
    m_paramDeviance(0),
    m_direction(0)
{
  /* */
}

void CentipedeHead::exec_impl()
{
  findSegments();

  int oldX = xPos();
  int oldY = yPos();

  doMove( randNotBlockedDir() );

  if ( oldX == xPos() && oldY == yPos() ) {
    switchHeadAndTail();
  }
  else {
    moveSegments( oldX, oldY );
  }
}


static void checkAddSegment( const ZZTEntity &ent,
                             std::vector<CentipedeSegment*> &segs )
{
  if ( ent.id() != ZZTEntity::CentipedeSegment ) return;
  segs.push_back( static_cast<CentipedeSegment*>(ent.thing()) );
}

void CentipedeHead::findSegments()
{
  int x, y;
  if ( m_body.empty() ) {
    x = xPos();
    y = yPos();
  }
  else {
    CentipedeSegment *segment = m_body.back();
    x = segment->xPos();
    y = segment->yPos();
  }

  bool addedBody;
  do
  {
    std::vector<CentipedeSegment*> segs;
    segs.reserve(4);
    checkAddSegment( board()->entity( x, y-1 ), segs );
    checkAddSegment( board()->entity( x, y+1 ), segs );
    checkAddSegment( board()->entity( x-1, y ), segs );
    checkAddSegment( board()->entity( x+1, y ), segs );

    const int segCount = segs.size();
    if ( segCount == 0 || segCount == 4 ) break;
    if ( segCount > 1 ) {
      random_shuffle ( segs.begin(), segs.end() );
    }

    addedBody = false;
    std::vector<CentipedeSegment*>::iterator iter;
    for ( iter = segs.begin(); iter != segs.end(); iter++ )
    {
      CentipedeSegment *segment = *iter;
      if ( !segment->head() ) {
        segment->setHead(this);
        m_body.push_back(segment);
        x = segment->xPos();
        y = segment->yPos();
        addedBody = true;
      }
    }
  }
  while ( addedBody );
}

void CentipedeHead::moveSegments( int oldX, int oldY )
{
  CentipedeBody::iterator iter;
  for ( iter = m_body.begin(); iter != m_body.end(); iter++ )
  {
    CentipedeSegment *segment = *iter;
    const int newX = oldX, newY = oldY;
    oldX = segment->xPos();
    oldY = segment->yPos();
    board()->moveThing( segment, newX, newY );
  }
}

void CentipedeHead::switchHeadAndTail()
{
  if (m_body.empty()) return;

  // in which a tail becomes a head and a head becomes a tail
  CentipedeSegment *tail = m_body.back();
  m_body.pop_back();
  m_body.reverse();
  m_body.push_back( tail );
  board()->switchThings( this, tail );
}

// -------------------------------------

CentipedeSegment::CentipedeSegment()
  : m_head(0)
{
  /* */
}

void CentipedeSegment::exec_impl()
{
  /* */
}

