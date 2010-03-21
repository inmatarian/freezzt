/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <string>

#include "debug.h"
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

std::string TextScrollModel::getTitleMessage() const
{
  return " ";
}

std::string TextScrollModel::getLineMessage( int line ) const
{
  return " ";
}

std::string TextScrollModel::getLineData( int line ) const
{
  return " ";
}

int TextScrollModel::lineCount() const
{
  return 1;
}

