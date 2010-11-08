/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef __ABSTRACT_FILE_MODEL_FACTORY_H__
#define __ABSTRACT_FILE_MODEL_FACTORY_H__

class AbstractScrollModel;
class AbstractFileModelFactory
{
  public:
    /// File Model for browsing zzt files
    virtual AbstractScrollModel * acquireFileListModel( const ZString &directory = "" ) = 0;

    /// File Model for browsing zzt files
    virtual AbstractScrollModel * currentFileListModel() = 0;

    /// File Model for browsing zzt files
    virtual void releaseFileListModel( AbstractScrollModel *model ) = 0;
};

#endif /* __ABSTRACT_FILE_MODEL_FACTORY_H__ */

