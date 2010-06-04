/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef TEXT_SCROLL_MODEL_H
#define TEXT_SCROLL_MODEL_H

#include "abstractScrollModel.h"

class TextScrollModelPrivate;
class TextScrollModel : public AbstractScrollModel
{
  public:
    TextScrollModel();
    virtual ~TextScrollModel();

    virtual ZString getTitleMessage() const;
    virtual ZString getLineMessage( int line ) const;
    virtual ZString getLineData( int line ) const;
    virtual int getLineColorFG( int line ) const;
    virtual Action getAction( int line ) const;
    virtual int lineCount() const;

  private:
    TextScrollModelPrivate *d;
};

#endif /* TEXT_SCROLL_MODEL_H */

