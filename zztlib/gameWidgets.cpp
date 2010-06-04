/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <string>
#include <vector>

#include "debug.h"
#include "defines.h"
#include "zstring.h"
#include "abstractPainter.h"
#include "gameWorld.h"
#include "gameWidgets.h"

using namespace Defines;

// color palette for all of the widgets
static const int textColor = BG_BLUE | WHITE;
static const int blinkTextColor = BLINK | BG_BLUE | WHITE;
static const int altTextColor = BG_BLUE | CYAN;
static const int altButTextColor = BG_BLUE | YELLOW;
static const int buttonColor = BG_GRAY | BLACK;
static const int altColor = BG_CYAN | BLACK;
static const int ammoColor = BG_BLUE | CYAN;
static const int torchColor = BG_BLUE | DARK_BROWN;

// -----------------------------------------------------------------

void GameWidget::drawButtonLine( AbstractPainter *painter,
                                 int column, int row,
                                 const ZString &buttxt, int butcolor,
                                 const ZString &valtxt, int txtcolor )
{
  const int butln = buttxt.length();
  const int txtln = valtxt.length();

  painter->drawText( column, row, txtcolor, "  " );
  painter->paintChar( column+2, row, ' ', butcolor );
  painter->drawText( column+3, row, butcolor, buttxt );
  painter->paintChar( column+3+butln, row, ' ', butcolor );
  painter->paintChar( column+4+butln, row, ' ', txtcolor );
  painter->drawText( column+5+butln, row, txtcolor, valtxt );
  int x = 5 + butln + txtln;
  while ( x < 20 ) {
    painter->paintChar( column+x, row, ' ', txtcolor );
    x++;
  }
}

void GameWidget::drawTextEditLine( AbstractPainter *painter, int column, int row,
                                   int size, int cursor, const ZString &txt,
                                   int txtcolor )
{
  painter->drawText( column, row, txtcolor, txt );
  const int txtln = txt.length();
  for ( int x=txtln; x < size; x++ ) {
    painter->paintChar( column+x, row, ' ', txtcolor );
  }

  char ascii = (cursor < txtln) ? txt.at(cursor) : ' ';
  int cursorcolor = txtcolor ^ BG_GRAY;
  painter->paintChar( column+cursor, row, ascii, cursorcolor );
}

void GameWidget::drawCenteredTextLine( AbstractPainter *painter, int column, int row,
                                       const ZString &txt, int txtcolor, int spacecolor )
{
  int x = 0;
  int txtln = txt.length();
  int tl = 10 - (txtln / 2);
  int tr = tl + txtln;

  for ( int x=0; x < tl; x++ ) {
    painter->paintChar( column+x, row, ' ', spacecolor );
  }

  painter->drawText( column + tl, row, txtcolor, txt );
  
  for ( int x=tr; x < 20; x++ ) {
    painter->paintChar( column+x, row, ' ', spacecolor );
  }
}

void GameWidget::drawItemLine( AbstractPainter *painter,
                               int column, int row,
                               int itempic, int itemcolor,
                               const ZString &txt, int txtcolor,
                               int itemval )
{
  painter->drawText( column, row, txtcolor, "  " );
  painter->paintChar( column+2, row, itempic, itemcolor );
  painter->drawText( column+3, row, txtcolor, txt );
  painter->drawNumber( column+12, row, txtcolor, itemval, 5 );
  painter->drawText( column+17, row, txtcolor, "   " );
}

void GameWidget::drawKeysLine( AbstractPainter *painter,
                               int column, int row,
                               int itempic, int itemcolor,
                               const ZString &txt, int txtcolor,
                               GameWorld *world )
{
  int k = 0;
  const int nokey = 0xf9;
  const int bg = itemcolor & 0xF0;
  painter->drawText( column, row, txtcolor, "  " );
  painter->paintChar( column+2, row, itempic, itemcolor );
  painter->drawText( column+3, row, txtcolor, txt );
  k = world->hasDoorKey(GameWorld::BLUE_DOORKEY) ? itempic : nokey;
  painter->paintChar( column+12, row, k, bg | BLUE );
  k = world->hasDoorKey(GameWorld::GREEN_DOORKEY) ? itempic : nokey;
  painter->paintChar( column+13, row, k, bg | GREEN );
  k = world->hasDoorKey(GameWorld::CYAN_DOORKEY) ? itempic : nokey;
  painter->paintChar( column+14, row, k, bg | CYAN );
  k = world->hasDoorKey(GameWorld::RED_DOORKEY) ? itempic : nokey;
  painter->paintChar( column+15, row, k, bg | RED );
  k = world->hasDoorKey(GameWorld::PURPLE_DOORKEY) ? itempic : nokey;
  painter->paintChar( column+16, row, k, bg | MAGENTA );
  k = world->hasDoorKey(GameWorld::YELLOW_DOORKEY) ? itempic : nokey;
  painter->paintChar( column+17, row, k, bg | YELLOW );
  k = world->hasDoorKey(GameWorld::WHITE_DOORKEY) ? itempic : nokey;
  painter->paintChar( column+18, row, k, bg | WHITE );
  painter->paintChar( column+19, row, ' ', txtcolor );
}

// -----------------------------------------------------------------

FramerateSliderWidget::FramerateSliderWidget()
  : m_value(4)
{
  setRow( 21 );
  setColumn( 60 );
}

void FramerateSliderWidget::doKeypress( int keycode, int unicode )
{
  int v = value();
  switch ( keycode )
  {
    case Z_Up:
    case Z_Left:
      v -= 1;
      break;
    case Z_Down:
    case Z_Right:
      v += 1;
      break;
    default: break;
  }

  setValue( v );
}

void FramerateSliderWidget::setValue( int value )
{
  m_value = boundInt( 0, value, 8 );
}

void FramerateSliderWidget::doPaint( AbstractPainter *painter )
{
  drawButtonLine( painter, column(), row(),
                  "S", buttonColor, "Speed", altButTextColor );

  drawCenteredTextLine( painter, column(), row()+1, str(),
                        ( focused() ? blinkTextColor : textColor ), textColor );

  drawCenteredTextLine( painter, column(), row()+2, "F....:....S",
                        altButTextColor, textColor );
}

static const char * framerate_slider_widget_str_table[11] =
{
  "v        ",
  " v       ",
  "  v      ",
  "   v     ",
  "    v    ",
  "     v   ",
  "      v  ",
  "       v ",
  "        v"
};

const char *FramerateSliderWidget::str() const
{
  return framerate_slider_widget_str_table[ value() ];
}

// -----------------------------------------------------------------

TextInputWidget::TextInputWidget()
  : cursor( 0 ),
    place( 0 )
{
  history.push_back( ZString() );
  setRow( 4 );
  setColumn( 64 );
}

void TextInputWidget::setFixedSuffix( const ZString &sufx )
{
  zdebug() << "TODO: Implement Fixed Suffixes in TextInputWidget";
}

void TextInputWidget::reset()
{
  userMessage.erase();
  fixedSuffix.erase();
  if ( !history.back().empty() ) {
    history.push_back( ZString() );
  }
  place = history.size() - 1;
  cursor = 0;
};

void TextInputWidget::doKeypress( int keypress, int unicode )
{
  switch ( keypress )
  {
    case Z_Backspace:
      if (cursor > 0) {
        userMessage.erase( cursor-1, 1 );
        cursor -= 1;
        history[place] = userMessage;
      }
      break;

    case Z_Delete:
      if (cursor < userMessage.length()) {
        userMessage.erase( cursor, 1 );
        history[place] = userMessage;
      }
      break;

    case Z_Up:
      if ( place > 0 ) {
        history[place] = userMessage;
        place -= 1;
        userMessage = history[place];
        cursor = userMessage.length();
      }
      break;

    case Z_Down:
      if ( place < (history.size()-1) ) {
        history[place] = userMessage;
        place += 1;
        userMessage = history[place];
        cursor = userMessage.length();
      }
      break;

    case Z_Left:
      if (cursor > 0 ) cursor -= 1;
      break;

    case Z_Right:
      if (cursor < userMessage.length()) cursor += 1;
      break;

    case Z_PageUp:
      place = 0;
      userMessage = history[place];
      cursor = userMessage.length();
      break;

    case Z_PageDown: 
      place = history.size() - 1;
      userMessage = history[place];
      cursor = userMessage.length();
      break;

    case Z_Home:
      cursor = 0;
      break;

    case Z_End:
      cursor = userMessage.length();
      break;

    case Z_Unicode: {
      const unsigned int userMaxLen = 12 - fixedSuffix.length();
      const char ascii = ( unicode >= 0x20 && unicode <= 0xff ) ? unicode : 0;
      if ( userMessage.length() < userMaxLen && ascii ) {
        userMessage.insert( cursor, 1, ascii );
        cursor += 1;
        history[place] = userMessage;
      }
      break;
    }
      
    default: break;
  }
}

void TextInputWidget::doPaint( AbstractPainter *painter )
{
  const int widgetColor = WHITE;
  drawTextEditLine( painter, column(), row(), 12, cursor, str(), widgetColor );
}

ZString TextInputWidget::str() const
{
  const unsigned int maxLen = 12 - fixedSuffix.length();
  ZString v = userMessage;
  while ( v.length() < maxLen ) v.push_back(' ');
  v += fixedSuffix;
  return v;
}

// -----------------------------------------------------------------

TitleModeInfoBarWidget::TitleModeInfoBarWidget()
{
  setRow(0);
  setColumn(60);
}

void TitleModeInfoBarWidget::doPaint( AbstractPainter *painter )
{
  int x = column();

  drawCenteredTextLine( painter, x, 0, "- - - - -", textColor, textColor );
  drawCenteredTextLine( painter, x, 1, "  FREE  ZZT  ", buttonColor, textColor );
  drawCenteredTextLine( painter, x, 2, "- - - - -", textColor, textColor );
  drawCenteredTextLine( painter, x, 3, " ", textColor, textColor );
  drawCenteredTextLine( painter, x, 4, " ", textColor, textColor );
  drawCenteredTextLine( painter, x, 5, " ", textColor, textColor );
  drawCenteredTextLine( painter, x, 6, " ", textColor, textColor );
  drawCenteredTextLine( painter, x, 7, " ", textColor, textColor );
  drawCenteredTextLine( painter, x, 8, " ", textColor, textColor );
  drawCenteredTextLine( painter, x, 9, " ", textColor, textColor );
  drawButtonLine( painter, x, 10, "P", buttonColor, "Play", textColor );
  drawCenteredTextLine( painter, x, 11, " ", textColor, textColor );
  drawCenteredTextLine( painter, x, 12, " ", textColor, textColor );
  drawCenteredTextLine( painter, x, 13, " ", textColor, textColor );
  drawCenteredTextLine( painter, x, 14, " ", textColor, textColor );
  drawCenteredTextLine( painter, x, 15, " ", textColor, textColor );
  drawCenteredTextLine( painter, x, 16, " ", textColor, textColor );
  drawCenteredTextLine( painter, x, 17, " ", textColor, textColor );
  drawCenteredTextLine( painter, x, 18, " ", textColor, textColor );
  drawCenteredTextLine( painter, x, 19, " ", textColor, textColor );
  drawCenteredTextLine( painter, x, 20, " ", textColor, textColor );
  framerateSliderWidget.doPaint( painter );
  drawCenteredTextLine( painter, x, 24, " ", textColor, textColor );
}

// -----------------------------------------------------------------

PlayModeInfoBarWidget::PlayModeInfoBarWidget()
{
  setRow(0);
  setColumn(60);
}

void PlayModeInfoBarWidget::doPaint( AbstractPainter *painter )
{
  int x = column();

  drawCenteredTextLine( painter, x, 0, "- - - - -", textColor, textColor );
  drawCenteredTextLine( painter, x, 1, "  FREE  ZZT  ", buttonColor, textColor );
  drawCenteredTextLine( painter, x, 2, "- - - - -", textColor, textColor );
  drawCenteredTextLine( painter, x, 3, " ", textColor, textColor );
  drawCenteredTextLine( painter, x, 4, " ", textColor, textColor );
  drawCenteredTextLine( painter, x, 5, " ", textColor, textColor );
  drawCenteredTextLine( painter, x, 6, " ", textColor, textColor );
  drawItemLine( painter, x, 7, 0x02, textColor,  "  Health:", textColor, world()->currentHealth() );
  drawItemLine( painter, x, 8, 0x84, ammoColor,  "    Ammo:", textColor, world()->currentAmmo() );
  drawItemLine( painter, x, 9, 0x9d, torchColor, " Torches:", textColor, world()->currentTorches() );
  drawItemLine( painter, x,10, 0x04, ammoColor,  "    Gems:", textColor, world()->currentGems() );
  drawItemLine( painter, x,11, 0x20, textColor,  "   Score:", textColor, world()->currentScore() );
  drawKeysLine( painter, x,12, 0x0c, textColor,  "    Keys:", textColor, world() );
  drawCenteredTextLine( painter, x, 13, " ", textColor, textColor );
  drawButtonLine( painter, x, 14, "T", buttonColor, "Torch", textColor );
  drawButtonLine( painter, x, 15, "B", altColor,    "Be quiet", textColor );
  drawButtonLine( painter, x, 16, "H", buttonColor, "Help", textColor );
  drawCenteredTextLine( painter, x, 17, " ", textColor, textColor );
  ZString arrows;
  arrows.append( 1, 0x18 );
  arrows.append( 1, 0x19 );
  arrows.append( 1, 0x1a );
  arrows.append( 1, 0x1b );
  drawButtonLine( painter, x, 18, arrows, altColor, "Move", textColor );
  ZString shiftArrows = "Shift";
  shiftArrows.append(arrows);
  drawButtonLine( painter, x, 19, shiftArrows, buttonColor, "Shoot", textColor );
  drawCenteredTextLine( painter, x, 20, " ", textColor, textColor );
  drawButtonLine( painter, x, 21, "S", buttonColor, "Save game", textColor );
  drawButtonLine( painter, x, 22, "P", altColor,    "Pause", textColor );
  drawButtonLine( painter, x, 23, "Q", buttonColor, "Quit", textColor );
  drawCenteredTextLine( painter, x, 24, " ", textColor, textColor );
}

