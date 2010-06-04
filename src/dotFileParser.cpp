/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

// Try to keep this file clean with regards to dependencies.

#include <cstdlib>
#include <algorithm>
#include <string>
#include <list>
#include <map>
#include <fstream>

#include "dotFileParser.h"

typedef std::list<std::string> StringList;
typedef std::map<std::string, StringList*> StringListMap;

namespace ParserStrings {
  const char *Whitespace = " \t\f\v\n\r";
  const char *Text = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
  const char *Comment = "#;";
  const char *NamespaceStart = "[";
  const char *NamespaceEnd = "]";
};

class DotFileParserPrivate
{
  public:
    StringList getStringList( const std::string &line );
    void getNamespace( const std::string &line );

  public:
    StringListMap map;
    std::string currNamespace;
};

/// tokenizes a string into a list of strings
StringList DotFileParserPrivate::getStringList( const std::string &line )
{
  using namespace std;

  // skip comment lines
  size_t comment = line.find_first_of(ParserStrings::Comment);
  if ( comment == string::npos ) comment = line.size();

  StringList strList;
  size_t start = 0;
  while ( start != string::npos )
  {
    start = line.find_first_of(ParserStrings::Text, start);
    if ( start == string::npos || start >= comment ) break;

    size_t end = line.find_first_not_of(ParserStrings::Text, start);
    string name = ( end != string::npos )
                  ? line.substr( start, end - start )
                  : line.substr( start );
    strList.push_back( name );
    start = end;
  }

  return strList;
}

void DotFileParserPrivate::getNamespace( const std::string &line )
{
  using namespace std;

  // make sure it's a namespace line.
  size_t start = line.find_first_of(ParserStrings::NamespaceStart);
  size_t end = line.find_first_of(ParserStrings::NamespaceEnd);
  if ( start == string::npos || end == string::npos ) return;

  // check if the namespace is named, or if it's the default.
  size_t nsStart = line.find_first_of(ParserStrings::Text, 0);
  size_t nsEnd = line.find_first_not_of(ParserStrings::Text, nsStart);
  if ( nsStart == string::npos || nsEnd == string::npos ) {
    // default, return to nothing.
    currNamespace.clear();
    return;
  }

  currNamespace = line.substr( nsStart, nsEnd-nsStart );
}

// -----------------------------------------------------------------

DotFileParser::DotFileParser()
  : d( new DotFileParserPrivate )
{
  /* */
}

DotFileParser::~DotFileParser()
{
  // clean up memory
  StringListMap::iterator iter;
  for( iter = d->map.begin(); iter != d->map.end(); ++iter ) {
    delete (*iter).second;
  }

  delete d;
}

void DotFileParser::load( const std::string &filename )
{
  using namespace std;
  fstream file( filename.c_str(), ios::in );
  if ( file.is_open() && file.good() )
  {
    int linesRead = 0;
    while ( !file.eof() && linesRead < 32768 )
    {
      string line;
      getline( file, line );
      linesRead += 1;

      // skip empty lines
      if ( line.empty() ) continue;

      // check if it's a namespace line
      size_t start = line.find_first_of(ParserStrings::NamespaceStart);
      if ( start != string::npos ) {
        d->getNamespace( line );
        continue;
      }

      // skip lines that are only whitespace
      start = line.find_first_of(ParserStrings::Text);
      if ( start == string::npos ) continue;

      // gather args
      StringList strList = d->getStringList( line );

      // skip empty/comment lines
      if ( strList.empty() ) continue;

      string key = strList.front();

      // skip empty/comment lines
      if ( key.empty() ) continue;

      // prepend current namespace
      if ( !d->currNamespace.empty() ) {
        string newKey = d->currNamespace;
        newKey.push_back('.');
        newKey += key;
        key = newKey;
      }

      // add to Map
      d->map[key] = new StringList(strList);

      // too large, bail out for security's sake.
      if ( d->map.size() >= 128 ) break;
    }

    file.close();
  }
}

std::string DotFileParser::getValue( const std::string &key, int index ) const
{
  StringListMap::iterator mapIter;
  mapIter = d->map.find(key);
  if ( mapIter == d->map.end() ) {
    return "";
  }

  StringList *strList = (*mapIter).second;
  if ( !strList ||
       index < 0 ||
       index >= (int) strList->size() )
  {
    return "";
  }

  // linear access, boo.
  StringList::iterator listIter;
  listIter = strList->begin();  
  for ( int i = 0; i < index; i++ ) {
    listIter++;
  }

  return (*listIter);
}

int DotFileParser::getInt( const std::string &key, int index, int defaultValue ) const
{
  std::string val = getValue( key, index );
  if ( val == "" ) return defaultValue;
  return atoi( val.c_str() );
}

int DotFileParser::getFromList( const std::string &key, int index,
                                const std::list<std::string> &list ) const
{
  std::string val = getValue( key, index );
  std::transform(val.begin(), val.end(), val.begin(), ::toupper);

  int i = 0; 
  std::list<std::string> safeList = list;
  while ( !safeList.empty() ) {
    std::string str = safeList.front();
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    if ( val == str ) return i;
    safeList.pop_front();
    i += 1;
  }
  return -1;
}

bool DotFileParser::getBool( const std::string &key, int index, bool defaultValue ) const
{
  StringList list;
  list.push_back("FALSE");
  list.push_back("TRUE");
  int v = getFromList( key, index, list );
  switch (v) {
    case 0: return false;
    case 1: return true;
    default: break;
  }
  return defaultValue;
}

