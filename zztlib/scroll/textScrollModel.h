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

    virtual std::string getTitleMessage() const;
    virtual std::string getLineMessage( int line ) const;
    virtual std::string getLineData( int line ) const;
    virtual int getLineColorFG( int line ) const;
    virtual int lineCount() const;

  private:
    TextScrollModelPrivate *d;
};

#endif /* TEXT_SCROLL_MODEL_H */

