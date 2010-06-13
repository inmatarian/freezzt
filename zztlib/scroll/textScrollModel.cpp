/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <string>
#include <vector>

#include "debug.h"
#include "zstring.h"
#include "textScrollModel.h"

struct LineData
{
  ZString message;
  ZString label;
  bool pretty;
  
  LineData()
    : pretty(false) { /* */ };

  LineData( const ZString &m, bool p = false )
    : message(m), pretty(p) { /* */ };

  LineData( const ZString &m, const ZString &l )
    : message(m), label(l), pretty(false) { /* */ };
};

// -------------------------------------

class TextScrollModelPrivate
{
  public:
    TextScrollModelPrivate( TextScrollModel *pSelf );

    std::vector<LineData> data;

  private:
    TextScrollModel *self;
};

TextScrollModelPrivate::TextScrollModelPrivate( TextScrollModel *pSelf )
  : self( pSelf )
{
  /* */
}

TextScrollModel::TextScrollModel()
  : d( new TextScrollModelPrivate(this) )
{
  /* */
}

TextScrollModel::~TextScrollModel()
{
  delete d;
  d = 0;
}

ZString TextScrollModel::getTitleMessage() const
{
  return " ";
}

ZString TextScrollModel::getLineMessage( int line ) const
{
  if ( line < 0 || line >= lineCount() ) {
    return ZString();
  }
  return d->data[line].message;
}

ZString TextScrollModel::getLineData( int line ) const
{
  if ( line < 0 || line >= lineCount() ) {
    return ZString();
  }
  return d->data[line].label;
}

bool TextScrollModel::isCentered( int line ) const
{
  if ( line < 0 || line >= lineCount() ) {
    return false;
  }
  return d->data[line].pretty;
}

bool TextScrollModel::isHighlighted( int line ) const
{
  if ( line < 0 || line >= lineCount() ) return false;
  if ( d->data[line].pretty ) return true;
  if ( d->data[line].label.empty() ) return false;
  return true;  
}

AbstractScrollModel::Action TextScrollModel::getAction( int line ) const
{
  if ( line < 0 || line >= lineCount() ) return AbstractScrollModel::Close;

  if ( !d->data[line].label.empty() ) return AbstractScrollModel::SendMessage;

  return AbstractScrollModel::Close;
}

int TextScrollModel::lineCount() const
{
  return d->data.size();
}

void TextScrollModel::appendPlainText( const ZString &message )
{
  d->data.push_back( LineData(message) );
}

void TextScrollModel::appendPrettyText( const ZString &message )
{
  d->data.push_back( LineData(message, true) );
}

void TextScrollModel::appendMenuText( const ZString &label, const ZString &message )
{
  d->data.push_back( LineData(message, label) );
}

