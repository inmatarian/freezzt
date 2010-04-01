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

    virtual std::string getTitleMessage() const;
    virtual std::string getLineMessage( int line ) const;
    virtual std::string getLineData( int line ) const;
    virtual int getLineColorFG( int line ) const;
    virtual Action getAction( int line ) const;
    virtual int lineCount() const;

    void setDirectory( const std::string &dir );

  private:
   FileListModelPrivate *d;
};

#endif /* FILE_LIST_MODEL_H */

