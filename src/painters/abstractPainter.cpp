/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <string>

#include "debug.h"
#include "abstractPainter.h"

void AbstractPainter::begin()
{
  m_blinkClock = currentTime();
  begin_impl();
}

void AbstractPainter::end()
{
  end_impl();
}

bool AbstractPainter::blinkOn() const
{
  // 2 hertz sound good?
  const int rate = 2;
  return ( ( m_blinkClock % (1000 / rate) ) / (500 / rate) );
};

void AbstractPainter::drawText( int x, int y,
                                unsigned char color,
                                const std::string &text )
{
  if (text.empty()) {
    return;
  }

  int tx = x;
  int len = text.length();
  for ( int i = 0; i < len; i++, tx++ ) {
    paintChar( tx, y, (unsigned char)(text.at(i)), color );
  }
}

