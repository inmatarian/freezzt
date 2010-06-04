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
#include "zstring.h"
#include "fileListModel.h"

struct FileTuple
{
  ZString name;
  ZString data;
  bool isDirectory;

  FileTuple( const ZString &n, const ZString d, bool i )
    : name(n), data(d), isDirectory(i) {/* */};

  bool operator<( const FileTuple &other ) const
  {
    if ( this->isDirectory && !other.isDirectory ) return true;
    if ( !this->isDirectory && other.isDirectory ) return false;
    return ( (this->name.lower()) < (other.name.lower()) );
  };
};

// -------------------------------------

class DirList
{
  public:
    DirList( const ZString &dirpath );
    ~DirList();

    bool next();
    const ZString & currentName() const;
    const ZString & currentPath() const;
    bool currentIsDirectory() const;
    bool currentIsValid() const;

  private:
    DIR *dir;
    struct dirent *ent;
    ZString path;
    ZString c_fullpath;
    ZString c_shortname;
    bool c_valid;
    bool c_dir;
};

DirList::DirList( const ZString &dirpath )
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
    c_shortname = ZString(ent->d_name);
    c_fullpath = path + "/" + ZString(ent->d_name);
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

const ZString & DirList::currentName() const
{
  return c_shortname;
}

const ZString & DirList::currentPath() const
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
    void makeDirList( const ZString &dir );
    static ZString cwd();

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

void FileListModelPrivate::makeDirList( const ZString &dir )
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

ZString FileListModelPrivate::cwd()
{
  char *c = new char[PATH_MAX];
  c[0] = 0;
  char *r = getcwd(c, PATH_MAX);
  assert(r);
  ZString d(c);
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

ZString FileListModel::getTitleMessage() const
{
  return ZString("Load World");
}

ZString FileListModel::getLineMessage( int line ) const
{
  if ( line < 0 || line >= lineCount() ) {
    return ZString();
  }

  return d->fileList.at(line).name;
}

ZString FileListModel::getLineData( int line ) const
{
  if ( line < 0 || line >= lineCount() ) {
    return ZString();
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

void FileListModel::setDirectory( const ZString &dir )
{
  if ( dir.empty() ) return;

  d->makeDirList( dir );
}
