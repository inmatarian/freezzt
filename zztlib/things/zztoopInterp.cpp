
#include <cassert>
#include <list>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <memory>
#include <vector>

#include "debug.h"
#include "zstring.h"
#include "scriptable.h"
#include "textScrollModel.h"
#include "zztoopInterp.h"

using namespace std;
using namespace ZZTOOP;
using namespace ZZTThing;

// -------------------------------------

namespace ZZTOOP {

// -------------------------------------

signed short length( const ProgramBank &program )
{
  const ProgramBank::size_type s = program.size();
  return ( s > 32767 ? 32767 : s );
}

// zzt's file format uses MSDOS 0x0d as the newline;
static const int zztNewLine = 0x0d;
static const char wholeLineDelimiter[] = { zztNewLine, 0 };
static const char labelDelimiters[] = { ' ', zztNewLine, 0 };
static const char crunchDelimiters[] = { ' ', ':', zztNewLine, 0 };
static const char moveDelimiters[] = { ' ', '@', '#', ':', '/', '?', '!', '$', '\'', zztNewLine, 0 };
static const char menuDelimiters[] = { ' ', ';', zztNewLine, 0 };

void getOneToken( const ProgramBank &program,
                  signed short &ip,
                  ZString &token,
                  const ZString &delimiter )
{
  token.clear();
  const int size = length(program);
  while ( ip < size ) {
    const unsigned char symbol = program.at( ip );
    if ( delimiter.find(symbol) != ZString::npos ) break;
    token.push_back( symbol );
    ip += 1;
  }
}

void getWholeLine( const ProgramBank &program, signed short &ip, ZString &token )
{
  getOneToken( program, ip, token, wholeLineDelimiter );
}

void seekForward( const ProgramBank &program, signed short &ip, const ZString &delimiter )
{
  const int size = length(program);
  while ( ip < size ) {
    const unsigned char symbol = program.at( ip );
    if ( delimiter.find(symbol) != ZString::npos ) break;
    ip += 1;
  }
}

bool seekToken( const ProgramBank &program, const ZString &delimiter,
                const ZString &label, signed short &targetIP, bool impliedLabels )
{
  // I wonder how slow this approach is.
  ZString capSeek = label.upper();

  // restart is an implied lable.
  if ( impliedLabels &&
       ( 0 == capSeek.compare("RESTART") ) )
  {
    targetIP = 0;
    return true;
  }

  const int size = length(program);
  signed short ip = 0;
  while ( ip < size )
  {
    const unsigned char symbol = program.at( ip );

    if ( delimiter.find(symbol) != ZString::npos ) {
      signed short delimiterIP = ip;
      ip++;
      if ( ip >= size ) break;
      ZString token;
      getOneToken( program, ip, token, labelDelimiters );
      if ( 0 == token.upper().compare( capSeek ) ) {
        targetIP = delimiterIP;
        return true;
      }
    }
    seekForward( program, ip, wholeLineDelimiter );
    if ( ip < size ) ip++;
  }
  return false;
}

// -------------------------------------

enum KILLENUM {
  FREEBIE,
  PROCEED,
  ENDCYCLE
};

KILLENUM zztoopError( ScriptableThing *thing, const ProgramBank &program,
                      signed short ip, const ZString &mesg )
{
  ZString rawLine;
  getWholeLine( program, ip, rawLine );
  zinfo() << "ZZTOOP ERROR:" << mesg << " --> " << rawLine;
  thing->setPaused(true);
  return ENDCYCLE;
}

KILLENUM showStrings( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  const int size = length(program);

  list<ZString> lines;
  int count = 0;

  // Collect strings
  while ( true )
  {
    ZString line;
    getWholeLine( program, ip, line );
    lines.push_back(line);
    count++;

    // end of program?
    if ( ip >= size ) break;

    // skip newline
    ip += 1;

    // end of program?
    if ( ip >= size ) break;

    // Stop at non-text commands
    const unsigned char symbol = program.at(ip);
    if ( symbol == '#' ) break;
  }

  // determine difference between Message and Scroll
  if ( count <= 1 ) {
    thing->showMessage( lines.front() );
  }
  else
  {
    TextScrollModel *model = new TextScrollModel;

    while ( !lines.empty() ) {
      ZString line = lines.front();
      lines.pop_front();
      if ( line.size() == 0 ) {
        model->appendPlainText(line);
      }
      else
      {
        switch ( line.at(0) )
        {
          case '$':
            if ( line.size() == 1 ) {
              model->appendPlainText( line );
            }
            else {
              model->appendPrettyText( line.substr(1) );
            }
            break;

          case '!':
            if ( line.size() == 1 ) {
              model->appendPlainText( line );
            }
            else {
              size_t pos = line.find(';');
              if ( pos == string::npos ) {
                model->appendPlainText( line );
              }
              else {
                ZString label = line.substr( 1, pos-1 );
                model->appendMenuText( label, line.substr(pos+1) );
              }
            }
            break;

          default:
            model->appendPlainText( line );
            break;
        }
      }
    }

    thing->showScroll( model );
    return ENDCYCLE;
  }

  return PROCEED;
}

KILLENUM sugarMove( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  const bool forcedMove = ( program.at(ip) == '/' );

  // Currently Unimplemented

  return ENDCYCLE;
}

KILLENUM executeCrunch( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  const int size = length(program);
  const signed short startLineIP = ip;

  KILLENUM ret = PROCEED;

  ip++; // point after the crunch.

  if ( ip >= size ) {
    return zztoopError( thing, program, startLineIP, "Error!" );
  }

  ZString token;
  getOneToken( program, ip, token, crunchDelimiters );
  token.toUpper();
  zdebug() << __FILE__ << ":" << __LINE__ << " --> " << token;

  if ( 0 == token.compare("BECOME") ) {
    /* */
  }
  else if ( 0 == token.compare("BIND") ) {
    /* */
  }
  else if ( 0 == token.compare("CHANGE") ) {
    /* */
  }
  else if ( 0 == token.compare("CHAR") ) {
    /* */
  }
  else if ( 0 == token.compare("CLEAR") ) {
    /* */
  }
  else if ( 0 == token.compare("CYCLE") ) {
    /* */
  }
  else if ( 0 == token.compare("DIE") ) {
    thing->setPaused( true );
    ret = ENDCYCLE;
  }
  else if ( 0 == token.compare("END") ) {
    thing->setPaused( true );
    ret = ENDCYCLE;
  }
  else if ( 0 == token.compare("ENDGAME") ) {
    /* */
  }
  else if ( 0 == token.compare("GIVE") ) {
    /* */
  }
  else if ( 0 == token.compare("GO") ) {
    /* */
  }
  else if ( 0 == token.compare("IDLE") ) {
    /* */
  }
  else if ( 0 == token.compare("IF") ) {
    /* */
  }
  else if ( 0 == token.compare("LOCK") ) {
    /* */
  }
  else if ( 0 == token.compare("PLAY") ) {
    if ( program.at(ip) != ' ' ) {
      return zztoopError( thing, program, startLineIP, "Error!" );
    }
    ip ++; // skip space
    ZString song;
    getWholeLine( program, ip, song );
    thing->execPlay( song );
  }
  else if ( 0 == token.compare("PUT") ) {
    /* */
  }
  else if ( 0 == token.compare("RESTART") ) {
    /* */
  }
  else if ( 0 == token.compare("RESTORE") ) {
    /* */
  }
  else if ( 0 == token.compare("SEND") ) {
    /* */
  }
  else if ( 0 == token.compare("SET") ) {
    /* */
  }
  else if ( 0 == token.compare("SHOOT") ) {
    /* */
  }
  else if ( 0 == token.compare("TAKE") ) {
    /* */
  }
  else if ( 0 == token.compare("THROWSTAR") ) {
    /* */
  }
  else if ( 0 == token.compare("TRY") ) {
    /* */
  }
  else if ( 0 == token.compare("UNLOCK") ) {
    /* */
  }
  else if ( 0 == token.compare("WALK") ) {
    /* */
  }
  else if ( 0 == token.compare("ZAP") ) {
    if ( program.at(ip) != ' ' ) {
      return zztoopError( thing, program, startLineIP, "Error!" );
    }
    ip ++; // skip space
    ZString label;
    getWholeLine( program, ip, label );
    thing->execZap( label );
  }
  else {
    // DEFAULT SEND COMMAND
  }

  // point past the newline.
  if ( ip < size && program.at(ip) == zztNewLine ) ip++;

  return ret;
}

KILLENUM parseNext( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  const int size = length(program);
  if ( ip >= size ) return ENDCYCLE;

  const unsigned char main_symbol = program.at( ip );
  switch ( main_symbol )
  {
    case zztNewLine: 
      ip++;
      return FREEBIE;

    case '@': // ignored during execution
    case '\'':
    case ':':
      zdebug() << __FILE__ << ":" << __LINE__ << " ignored type";
      ip++;
      seekForward( program, ip, wholeLineDelimiter );
      if ( ip < size ) ip++;
      return FREEBIE;

    case '#':
      zdebug() << __FILE__ << ":" << __LINE__ << " crunch";
      return executeCrunch( thing, program, ip );

    case '?':
    case '/': // currently unimplemented
      zdebug() << __FILE__ << ":" << __LINE__ << " sugar move";
      return sugarMove( thing, program, ip );

    case '!':
    case '$':
    case ' ':
    default:
      zdebug() << __FILE__ << ":" << __LINE__ << " strings";
      return showStrings( thing, program, ip );
  }

  return FREEBIE;
}

void run( ScriptableThing *thing, const ProgramBank &program, int cycles )
{
  signed short ip = thing->instructionPointer();
  while ( cycles > 0 )
  {
    if ( thing->paused() ) break;

    if ( ip >= length(program) ) break;

    KILLENUM kill = parseNext( thing, program, ip );

    if ( kill == ENDCYCLE ) break;
    else if ( kill == PROCEED ) cycles --;
  }

  thing->setInstructionPointer( ip );
}

}; //namespace

// -------------------------------------

void Interpreter::setProgram( const unsigned char *stream, int length )
{
  assert( length >= 0 );

  if ( length == 0 ) return;

  program.resize( length );
  std::copy( stream, stream + length, program.begin() );
}

ZString Interpreter::getObjectName() const
{
  if ( length(program) < 2 ) return "";
  if ( program.at(0) != '@' ) return "";

  signed short ip = 0;
  ZString name;
  getWholeLine( program, ip, name );
  return name;
}

void Interpreter::run( ScriptableThing *thing, int cycles )
{
  ZZTOOP::run( thing, program, cycles );
}

void Interpreter::seekLabel( ScriptableThing *thing, const ZString &label )
{
  signed short ip = thing->instructionPointer();

  if ( seekToken( program, ":", label, ip, true ) ) {
    thing->setInstructionPointer(ip);
    thing->setPaused( false );
    zdebug() << "Found label --> " << label;
  }
}

void Interpreter::zapLabel( const ZString &label )
{
  signed short ip = 0;
  if ( seekToken( program, ":", label, ip, false ) ) {
    program[ip] = '\'';
  }
}

void Interpreter::restoreLabel( const ZString &label )
{
  signed short ip = 0;
  if ( seekToken( program, "\'", label, ip, false ) ) {
    program[ip] = ':';
  }
}

