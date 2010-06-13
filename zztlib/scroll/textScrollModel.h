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
    virtual bool isCentered( int line ) const;
    virtual bool isHighlighted( int line ) const;
    virtual Action getAction( int line ) const;
    virtual int lineCount() const;

    void appendPlainText( const ZString &message );
    void appendPrettyText( const ZString &message );
    void appendMenuText( const ZString &label, const ZString &message );

  private:
    TextScrollModelPrivate *d;
};

#endif /* TEXT_SCROLL_MODEL_H */

