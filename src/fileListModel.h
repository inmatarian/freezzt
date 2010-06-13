/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef FILE_LIST_MODEL_H
#define FILE_LIST_MODEL_H

#include "abstractScrollModel.h"

class FileListModelPrivate;
class FileListModel : public AbstractScrollModel
{
  public:
    FileListModel();
    virtual ~FileListModel();

    virtual ZString getTitleMessage() const;
    virtual ZString getLineMessage( int line ) const;
    virtual ZString getLineData( int line ) const;
    virtual bool isCentered( int line ) const;
    virtual bool isHighlighted( int line ) const;
    virtual Action getAction( int line ) const;
    virtual int lineCount() const;

    void setDirectory( const ZString &dir );

  private:
   FileListModelPrivate *d;
};

#endif /* FILE_LIST_MODEL_H */

