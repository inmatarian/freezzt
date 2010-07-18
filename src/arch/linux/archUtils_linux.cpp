
#include <string>
#include <cstdlib>

#include "archUtils.h"

// Unless a better spec comes up, lets try to follow the XDG spec for
// stashing config files and save files. That means look for these:
//   $XDG_DATA_HOME:   $HOME/.local/share -- Save files
//   $XDG_CONFIG_HOME: $HOME/.config -- Config files
//   $XDG_CACHE_HOME:  $HOME/.cache -- Backup?
//   $XDG_DATA_DIRS:   /usr/local/share/:/usr/share/
//   $XDG_CONFIG_DIRS: /etc/xdg

std::string ArchUtils::findConfigFile( const std::string &name )
{
  std::string location;
  const char freezzt[] = "freezzt";

  char *config = getenv("XDG_CONFIG_HOME");
  if ( config ) {
    location = config;
    location += "/";
    location += freezzt;
    location += "/";
    location += name;
    return location;
  }

  config = getenv("HOME");
  if ( config ) {
    location = config;
    location += "/";
    location += ".config/";
    location += freezzt;
    location += "/";
    location += name;
    return location;
  }

  // no xdg compatible location found.
  return name;
}

