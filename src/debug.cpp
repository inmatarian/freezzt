// Insert copyright and license information here.

#include <iostream>
#include <string>
#include "debug.h"

using namespace std;

DebuggingStream *DebuggingStream::m_instance = 0;

DebuggingStream *DebuggingStream::instance()
{
  if ( !m_instance ) {
    m_instance = new DebuggingStream();
  }
  return m_instance;
}

DebuggingStream &DebuggingStream::message( const std::string &mesg, LogLevel priority )
{
  if ( level() < priority ) {
    disable();
    return *this;
  }

  enable();
  (*this) << endl << mesg;
  return *this;
}

DebuggingStream &zinfo()
{
  return DebuggingStream::instance()->message("INFO: ", DebuggingStream::INFORMATIVE);
}

DebuggingStream &zdebug()
{
  return DebuggingStream::instance()->message("DEBUG: ", DebuggingStream::DEBUGGING);
}

DebuggingStream &zwarn()
{
  return DebuggingStream::instance()->message("WARNING: ", DebuggingStream::WARNINGS);
}

DebuggingStream &zerror()
{
  return DebuggingStream::instance()->message("ERROR: ", DebuggingStream::ERRORS);
}

