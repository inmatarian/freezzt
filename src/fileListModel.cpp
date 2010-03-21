/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <string>
#include <vector>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>

#include "debug.h"
#include "fileListModel.h"

struct FilePair
{
  std::string name;
  std::string data;

  FilePair( const std::string &n, const std::string d ) : name(n), data(d) {/* */};
  bool operator<( const FilePair &other ) const
  {
    return this->name < other.name;
  };
};

class FileListModelPrivate
{
  public:
    FileListModelPrivate( FileListModel *pSelf );
    void makeDirList( const std::string &dir );
    static std::string cwd();

  public:
    std::vector<FilePair> fileList;

  private:
    FileListModel *self;
};

FileListModelPrivate::FileListModelPrivate( FileListModel *pSelf )
  : self( pSelf )
{
  /* */
}

void FileListModelPrivate::makeDirList( const std::string &dir )
{
  DIR *d = 0;
  struct dirent *e = 0;

  fileList.clear();

  d = opendir( dir.c_str() );
  if ( !d ) {
    zwarn() << "Error listing" << dir;
    return;
  }

  while ( (e = readdir( d )) ) {
    fileList.push_back( FilePair( e->d_name, e->d_name ) );
  }
  closedir( d );

  sort( fileList.begin(), fileList.end() );
}

std::string FileListModelPrivate::cwd()
{
  char c[128];
  getcwd(c, 128);
  c[127] = 0;
  std::string d(c);
  return d;
}

FileListModel::FileListModel()
  : d( new FileListModelPrivate(this) )
{
  d->makeDirList( d->cwd() ); 
}

FileListModel::~FileListModel()
{
  delete d;
  d = 0;
}

std::string FileListModel::getTitleMessage() const
{
  return std::string("Load World");
}

std::string FileListModel::getLineMessage( int line ) const
{
  if ( line < 0 || line >= lineCount() ) {
    return std::string();
  }

  return d->fileList.at(line).name;
}

std::string FileListModel::getLineData( int line ) const
{
  if ( line < 0 || line >= lineCount() ) {
    return std::string();
  }

  return d->fileList.at(line).data;
}

int FileListModel::lineCount() const
{
  return d->fileList.size();
}

