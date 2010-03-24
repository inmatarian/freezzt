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
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>

#include "debug.h"
#include "fileListModel.h"

struct FilePair
{
  std::string name;
  std::string data;
  bool isDirectory;

  FilePair( const std::string &n, const std::string d, bool i )
    : name(n), data(d), isDirectory(i) {/* */};

  bool operator<( const FilePair &other ) const
  {
    if ( this->isDirectory && !other.isDirectory ) return true;
    if ( !this->isDirectory && other.isDirectory ) return false;

    std::string left;
    std::transform( this->name.begin(), this->name.end(),
                    std::back_inserter(left), tolower );

    std::string right;
    std::transform( other.name.begin(), other.name.end(), 
                    std::back_inserter(right), tolower );

    return left < right;
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

  while ( (e = readdir( d )) )
  {
    struct stat inode;
    std::string path = dir + "/" + std::string(e->d_name);
    bool yes = false;
    bool dir = false;
    if ( stat(path.c_str(), &inode) == 0 ) {
      if ( S_ISDIR(inode.st_mode) ) {
        yes = true;
        dir = true;
      }
      else if ( S_ISREG(inode.st_mode) ) {
        yes = true;
        dir = false;
      }
    }

    if (yes) {
      fileList.push_back( FilePair( e->d_name, path, dir ) );
    }
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

int FileListModel::getLineColorFG( int line ) const
{
  if ( line < 0 || line >= lineCount() ) {
    return 0;
  }
  
  return d->fileList.at(line).isDirectory ? 0x0f : 0x0e;
}

int FileListModel::lineCount() const
{
  return d->fileList.size();
}

