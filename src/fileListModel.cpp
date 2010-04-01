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
#include <cassert>

#include "debug.h"
#include "fileListModel.h"

struct FileTuple
{
  std::string name;
  std::string data;
  bool isDirectory;

  FileTuple( const std::string &n, const std::string d, bool i )
    : name(n), data(d), isDirectory(i) {/* */};

  bool operator<( const FileTuple &other ) const
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
    const std::string & currentName() const;
    const std::string & currentPath() const;
    bool currentIsDirectory() const;
    bool currentIsValid() const;

  private:
    DIR *dir;
    struct dirent *ent;
    std::string path;
    std::string c_fullpath;
    std::string c_shortname;
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
    c_shortname = std::string(ent->d_name);
    c_fullpath = path + "/" + std::string(ent->d_name);
    if ( stat(c_fullpath.c_str(), &inode) == 0 &&
         c_shortname != "." )
    {
      if ( S_ISDIR(inode.st_mode) ) {
        c_valid = true;
        c_dir = true;
        c_shortname += "/";
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

const std::string & DirList::currentName() const
{
  return c_shortname;
}

const std::string & DirList::currentPath() const
{
  return c_fullpath;
}

bool DirList::currentIsDirectory() const
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
    std::vector<FileTuple> fileList;

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

    fileList.push_back( FileTuple( dirlist.currentName(),
                                   dirlist.currentPath(),
                                   dirlist.currentIsDirectory() ) );
  }

  sort( fileList.begin(), fileList.end() );
}

std::string FileListModelPrivate::cwd()
{
  char *c = new char[PATH_MAX];
  c[0] = 0;
  char *r = getcwd(c, PATH_MAX);
  assert(r);
  std::string d(c);
  delete[] c;
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

AbstractScrollModel::Action FileListModel::getAction( int line ) const
{
  if ( line < 0 || line >= lineCount() ) {
    return None;
  }
  
  return d->fileList.at(line).isDirectory
         ? ChangeDirectory
         : LoadFile;
}

int FileListModel::lineCount() const
{
  return d->fileList.size();
}

void FileListModel::setDirectory( const std::string &dir )
{
  if ( dir.empty() ) return;

  d->makeDirList( dir );
}
