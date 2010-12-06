
#include <cstdlib>
#include <cctype>
#include <cerrno>
#include <algorithm>
#include <string>
#include <list>
#include "zstring.h"

void ZString::toUpper()
{
  std::transform(begin(), end(), begin(), ::toupper);
}

ZString ZString::upper() const
{
  ZString next;
  std::transform(begin(), end(), std::back_inserter( next ), ::toupper);
  return next;
}

void ZString::toLower()
{
  std::transform(begin(), end(), begin(), ::tolower);
}

ZString ZString::lower() const
{
  ZString next;
  std::transform(begin(), end(), std::back_inserter( next ), ::tolower);
  return next;
}

bool ZString::isNumber() const
{
  for ( const_iterator iter = begin(); iter < end(); iter++ ) {
    if ( !isdigit( *iter ) ) return false;
  }

  return true;
}

unsigned int ZString::uint( bool *error, int base ) const
{
  unsigned long int x = strtoul( c_str(), 0, base );
  if ( error ) {
    *error = ( errno == ERANGE ) ? true : false;
  }
  return x;
}

int ZString::sint( bool *error, int base ) const
{
  long int x = strtol( c_str(), 0, base );
  if ( error ) {
    *error = ( errno == ERANGE ) ? true : false;
  }
  return x;
}

signed short ZString::word( bool *error, int base ) const
{
  return sint( error, base );
}

unsigned char ZString::byte( bool *error, int base ) const
{
  return uint( error, base );
}

std::list<ZString> ZString::split( const ZString& delim )
{
  std::list<ZString> tokens;

  size_type left = find_first_not_of( delim, 0 );
  size_type right = find_first_of( delim, left );

  while ( left != npos || right != npos )
  {
    tokens.push_back( substr( left, right - left ) );
    left = find_first_not_of( delim, right );
    right = find_first_of( delim, left );
  }
  return tokens;
}

