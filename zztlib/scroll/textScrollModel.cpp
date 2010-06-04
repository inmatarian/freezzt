/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <string>

#include "debug.h"
#include "zstring.h"
#include "textScrollModel.h"

class TextScrollModelPrivate
{
  public:
    TextScrollModelPrivate( TextScrollModel *pSelf );

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
  return " ";
}

ZString TextScrollModel::getLineData( int line ) const
{
  return " ";
}

int TextScrollModel::getLineColorFG( int line ) const
{
  return 0x0e;
}

AbstractScrollModel::Action TextScrollModel::getAction( int line ) const
{
  return AbstractScrollModel::Close;
}

int TextScrollModel::lineCount() const
{
  return 1;
}

