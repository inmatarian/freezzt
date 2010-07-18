#ifndef ARCH_UTILS_H
#define ARCH_UTILS_H

namespace ArchUtils
{
  /// Returns the full path of the preferred config file to load.
  std::string findConfigFile( const std::string &name );
};

#endif // ARCH_UTILS_H

