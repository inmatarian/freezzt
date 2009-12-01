// Insert copyright and license information here.

#include <cstdlib>
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
};

class DotFileParserPrivate
{
  public:
    StringListMap map;
};

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
    while ( !file.eof() )
    {
      string line;
      getline( file, line );

      // skip empty lines
      if ( line.empty() ) continue;

      // skip comment lines
      size_t comment = line.find_first_of(ParserStrings::Comment);
      if ( comment == 0 ) continue;

      // skip lines that are only whitespace
      size_t start = line.find_first_of(ParserStrings::Text);
      if ( start == string::npos ) continue;

      // gather args
      StringList *strList = new StringList;
      while ( start != string::npos )
      {
        start = line.find_first_of(ParserStrings::Text, start);
        size_t end = line.find_first_not_of(ParserStrings::Text, start);
        string name = ( end != string::npos )
                      ? line.substr( start, end - start )
                      : line.substr( start );
        strList->push_back( name );
        start = end;
      }

      // add to Map
      d->map[strList->front()] = strList;
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
  if ( index < 0 || index >= (int) strList->size() ) {
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

int DotFileParser::getInt( const std::string &key, int index ) const
{
  return atoi( getValue( key, index ).c_str() );
}

