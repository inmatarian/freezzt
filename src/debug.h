// Insert copyright and license information here.

#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>

#define  DEBUGGING_ENABLED  1

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

    static DebuggingStream *instance();

    DebuggingStream &message( const std::string &mesg, LogLevel priority );

    void setLevel( LogLevel level ) { m_level = level; };
    LogLevel level() const { return m_level; };

    void enable() { m_enabled = true; };
    void disable() { m_enabled = false; };

    // magic
    template <class T>
    inline DebuggingStream &operator<<(const T &inVal)
    {
      if (!m_enabled) return *this;
#if DEBUGGING_ENABLED
      std::cout << inVal;
#endif
      return *this;
    }
 
    // magic
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

DebuggingStream &zinfo();
DebuggingStream &zdebug();
DebuggingStream &zwarn();
DebuggingStream &zerror();

#endif // DEBUG_H

