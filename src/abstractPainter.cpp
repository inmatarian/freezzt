// Insert copyright and license information here.

#include <string>

#include "debug.h"
#include "abstractPainter.h"

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



