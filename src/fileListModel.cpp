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

// -------------------------------------

class DirList
{
  public:
    DirList( const std::string &dirpath );
    ~DirList();

    bool next();
    std::string currentName() const;
    std::string currentPath() const;
    bool currentIsDirectory();
    bool currentIsValid() const;

  private:
    DIR *dir;
    struct dirent *ent;
    std::string path;
    std::string c_fullpath;
    bool c_valid;
    bool c_dir;
};

DirList::DirList( const std::string &dirpath )
  : dir(0), ent(0), path(dirpath), c_valid(false), c_dir(false)
{
  dir = opendir( path.c_str() );
  if ( !dir ) zwarn() << "Error listing" << dir;
}

DirList::~DirList()
{
  if ( dir ) closedir( dir );
}

bool DirList::next()
{
  if ( !dir ) return false;

  ent = readdir( dir );
  if ( ent )
  {
    struct stat inode;
    c_fullpath = path + "/" + std::string(ent->d_name);
    if ( stat(c_fullpath.c_str(), &inode) == 0 ) {
      if ( S_ISDIR(inode.st_mode) ) {
        c_valid = true;
        c_dir = true;
      }
      else if ( S_ISREG(inode.st_mode) ) {
        c_valid = true;
        c_dir = false;
      }
      else {
        c_valid = false;
        c_dir = false;
      }
    }
    else {
      c_valid = false;
      c_dir = false;
    }
    return true;
  }

  return false;
}

std::string DirList::currentName() const
{
  return ent->d_name;
}

std::string DirList::currentPath() const
{
  return c_fullpath;
}

bool DirList::currentIsDirectory()
{
  return c_dir;
}

bool DirList::currentIsValid() const
{
  return c_valid;
}

// -------------------------------------

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
  fileList.clear();
  DirList dirlist(dir);

  while ( dirlist.next() )
  {
    if ( !dirlist.currentIsValid() ) continue;

    fileList.push_back( FilePair( dirlist.currentName(),
                                  dirlist.currentPath(),
                                  dirlist.currentIsDirectory() ) );
  }

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

// -------------------------------------

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

