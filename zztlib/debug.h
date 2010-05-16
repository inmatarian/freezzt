/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <ostream>
#include <iostream>
#include <sstream>
#define  DEBUGGING_ENABLED  1

/// debugging class for writing to stdout
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

    static void setGlobalLevel( LogLevel level ) { m_globalLogLevel = level; };
    static LogLevel globalLevel() { return m_globalLogLevel; };

  public:
    inline DebuggingStream( LogLevel level )
      : m_level(level) { /* */ };

    ~DebuggingStream();

    /// restrains logging level
    inline bool isLoggable() const { return (m_level <= m_globalLogLevel); };

    /// magic
    template <class T>
    inline DebuggingStream &operator<<(const T &inVal)
    {
      if (!isLoggable()) return *this;
      std::stringstream stream;
      stream << inVal << " ";
      m_buffer.append(stream.str());
      return *this;
    }
 
    /// magic
    inline DebuggingStream &operator<<(std::ostream& (*inVal)(std::ostream&))
    {
      if (!isLoggable()) return *this;
      std::stringstream stream;
      stream << inVal << " ";
      m_buffer.append(stream.str());
      return *this;
    }

  private:
    LogLevel m_level;
    std::string m_buffer;
    static LogLevel m_globalLogLevel;
};

/// convienience function, prefer using this 
inline DebuggingStream zinfo()
{
  return ( DebuggingStream(DebuggingStream::INFORMATIVE) );
}

/// convienience function, prefer using this 
inline DebuggingStream zdebug()
{
  return ( DebuggingStream(DebuggingStream::DEBUGGING) );
}

/// convienience function, prefer using this 
inline DebuggingStream zwarn()
{
  return ( DebuggingStream(DebuggingStream::WARNINGS) );
}

/// convienience function, prefer using this 
inline DebuggingStream zerror()
{
  return ( DebuggingStream(DebuggingStream::ERRORS) );
}

inline std::ostream &zout()
{
  return std::cout;
}

#endif // DEBUG_H

