/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <iostream>
#include <string>
#include "debug.h"

DebuggingStream::LogLevel DebuggingStream::m_globalLogLevel = DebuggingStream::NONE;

DebuggingStream::~DebuggingStream()
{
#if DEBUGGING_ENABLED
  if ( isLoggable() ) {
    switch (m_level) {
      case ERRORS: std::cout << "ERROR: "; break;
      case WARNINGS: std::cout << "WARNING: "; break;
      case DEBUGGING: std::cout << "DEBUG: "; break;
      case INFORMATIVE: std::cout << "INFO: "; break;
      default: break;
    }
    std::cout << m_buffer << "\n";
  }
#endif
}

