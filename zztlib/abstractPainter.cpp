/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <string>
#include <sstream>

#include "debug.h"
#include "zstring.h"
#include "abstractPainter.h"

void AbstractPainter::begin()
{
  // 2 hertz sound good?
  const int rate = 2;
  int clock = currentTime();
  m_blinkOn = ( clock % (1000 / rate) ) / (500 / rate);

  begin_impl();
}

void AbstractPainter::end()
{
  end_impl();
}

bool AbstractPainter::blinkOn() const
{
  return m_blinkOn;
};

void AbstractPainter::drawText( int x, int y,
                                unsigned char color,
                                const ZString &text )
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

void AbstractPainter::drawNumber( int x, int y, unsigned char color,
                                  int number, int size, int justified )
{
  std::ostringstream buffer;
  buffer << number;
  ZString str = buffer.str();

  const int prejustified = str.length();
  if ( prejustified < size) {
    switch ( justified ) {
      case RIGHT:
        str.insert( 0, (size - prejustified), ' ' );
        break;

      case LEFT:
      default:
        str.append( (size - prejustified), ' ' );
        break;
    }
  }

  drawText( x, y, color, str );
}

