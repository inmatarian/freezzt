/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <cstdlib>
#include <cassert>

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
  : AbstractListThing(),
    m_paramIntel(0),
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
                             AbstractListThing **segs,
                             int *segCount )
{
  if ( ent.id() == ZZTEntity::CentipedeSegment ) {
    segs[*segCount] = static_cast<AbstractListThing*>( ent.thing() );
    *segCount += 1;
  }
};

void CentipedeHead::findSegments()
{
  AbstractListThing *node = this;
  AbstractListThing *prevNode = 0;

  while ( node )
  {
    if ( node->next() )
    {
      // Travel down body
      prevNode = node;
      node = node->next();
    }
    else
    {
      // Add to tail
      int segCount = 0;
      AbstractListThing *segs[4];
      for ( int i = 0; i < 4; i++ ) segs[i] = 0;

      const int x = node->xPos(), y = node->yPos();
      checkAddSegment( board()->entity( x, y-1 ), segs, &segCount );
      checkAddSegment( board()->entity( x, y+1 ), segs, &segCount );
      checkAddSegment( board()->entity( x-1, y ), segs, &segCount );
      checkAddSegment( board()->entity( x+1, y ), segs, &segCount );

      if ( segCount == 0 || segCount == 4 ) return;

      int attempts = segCount;
      int getSeg = 0;
      if ( segCount > 1 ) getSeg = rand() % segCount;

      // set the while loop's terminator 
      AbstractListThing *currNode = node;
      node = 0;

      while ( attempts > 0 )
      {
        AbstractListThing *segment =
            static_cast<AbstractListThing*>( segs[getSeg] );

        if ( !segment->previous() )
        {
          // no previous segment? you're mine.
          AbstractListThing::link( currNode, segment );
          prevNode = currNode;
          node = segment;
          break;
        }
        
        getSeg = ( getSeg + 1 ) % segCount;
        attempts -= 1;
      }
    }
  }
}

void CentipedeHead::moveSegments( int oldX, int oldY )
{
  if (!next()) return;

  AbstractListThing *node = next();
  while ( node )
  {
    int newX = oldX;
    int newY = oldY;
    oldX = node->xPos();
    oldY = node->yPos();
    board()->moveThing( node, newX, newY );
    node = node->next();
  }
}

void CentipedeHead::switchHeadAndTail()
{
  // in which a tail becomes a head and a head becomes a tail
  AbstractListThing *node = this, *tail = 0;
  while (node) {
    tail = node;
    node = node->next();
  }

  // don't switch with self.
  if ( this == tail ) return;

  board()->switchThings( this, tail );

  AbstractListThing *tailPrev = tail->previous();
  AbstractListThing *headNext = next();

  // don't switch links if already pointing at tail
  if ( headNext == tail ) return;

  // switch entire link list, except head and tail
  node = headNext;
  while ( node != tail ) {
    AbstractListThing *nextNode = node->next();
    node->setNext( node->previous() );
    node->setPrevious( nextNode );
    node = nextNode;
  }
 
  // fix head and tail links
  assert( headNext->next() == this );
  assert( tailPrev->previous() == tail );
  AbstractListThing::link( this, tailPrev );
  AbstractListThing::link( headNext, tail );
}

// -------------------------------------

CentipedeSegment::CentipedeSegment()
  : AbstractListThing()
{
  /* */
}

void CentipedeSegment::exec_impl()
{
  /* */
}

