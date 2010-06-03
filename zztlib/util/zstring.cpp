
#include <stdlib.h>
#include <algorithm>
#include <string>
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

