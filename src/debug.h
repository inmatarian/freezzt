/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>

#define  DEBUGGING_ENABLED  1

/// debugging singleton class for writing to stdout
class DebuggingStream
{
  public:
    enum LogLevel {
      NONE,
      ERRORS,
      WARNINGS,
      DEBUGGING,
      INFORMATIVE
    };

    /// singleton accessor
    static DebuggingStream *instance();

    /// message to be written to stdout
    DebuggingStream &message( const std::string &mesg, LogLevel priority );

    /// Sets the height of the level 
    void setLevel( LogLevel level ) { m_level = level; };

    /// accessor for current log level
    LogLevel level() const { return m_level; };

    /// enable writing, used with log level setting
    void enable() { m_enabled = true; };

    /// disable writing, used with log level setting
    void disable() { m_enabled = false; };

    /// magic
    template <class T>
    inline DebuggingStream &operator<<(const T &inVal)
    {
      if (!m_enabled) return *this;
#if DEBUGGING_ENABLED
      std::cout << inVal;
#endif
      return *this;
    }
 
    /// magic
    inline DebuggingStream &operator<<(std::ostream& (*inVal)(std::ostream&))
    {
      if (!m_enabled) return *this;
#if DEBUGGING_ENABLED
      std::cout << inVal;
#endif
      return *this;
    }

  private:
    DebuggingStream()
      : m_level(INFORMATIVE),
        m_enabled(true)
    { /* */ };

    static DebuggingStream *m_instance;
    LogLevel m_level;
    bool m_enabled;
};

/// convienience function, prefer using this 
DebuggingStream &zinfo();

/// convienience function, prefer using this 
DebuggingStream &zdebug();

/// convienience function, prefer using this 
DebuggingStream &zwarn();

/// convienience function, prefer using this 
DebuggingStream &zerror();

#endif // DEBUG_H

