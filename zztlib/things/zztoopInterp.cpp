
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

namespace Crunch {
  enum Code {
    None,
    Become,
    Bind,
    Change,
    Char,
    Clear,
    Cycle,
    Die,
    End,
    Endgame,
    Give,
    Go,
    Idle,
    If,
    Lock,
    Play,
    Put,
    Restart,
    Restore,
    Send,
    Set,
    Shoot,
    Take,
    Throwstar,
    Try,
    Unlock,
    Walk,
    Zap
  };
};

namespace Command {
  enum Code {
    None,
    Name,
    Label,
    Remark,
    Text,
    PrettyText,
    Menu,
    Move,
    Try,
    Crunch
  };
};

// -------------------------------------

signed short length( const ProgramBank &program )
{
  const ProgramBank::size_type s = program.size();
  return ( s > 32767 ? 32767 : s );
}

inline bool verifyTokens( const list<ZString> &tokens,
                          unsigned int min, unsigned int max = 0 )
{
  if ( (max > 0) && (tokens.size()>max) ) return false;
  return ( tokens.size() >= min );
}

static void throwError( ScriptableThing *thing, const ZString &text )
{
  zinfo() << "ZZTOOP ERROR:" << text;
  thing->setPaused(true);
}

// zzt's file format uses MSDOS 0x0d as the newline;
static const int zztNewLine = 0x0d;
static const char wholeLineDelimiter[] = { zztNewLine, 0 };
static const char crunchDelimiters[] = { ' ', ':', zztNewLine, 0 };
static const char moveDelimiters[] = { ' ', '@', '#', ':', '/', '?', '!', '$', '\'', zztNewLine, 0 };
static const char menuDelimiters[] = { ' ', ';', zztNewLine, 0 };

static void getOneToken( const ProgramBank &program,
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

static void getWholeLine( const ProgramBank &program, signed short &ip, ZString &token )
{
  getOneToken( program, ip, token, wholeLineDelimiter );
}

static void parseTokens( const ProgramBank &program,
                         signed short &ip,
                         Command::Code &comType,
                         list<ZString> &tokens,
                         ZString &rawLine )
{
  using namespace ZZTOOP;
  const int size = length(program);
  tokens.clear();
  comType = Command::None;
  if ( ip >= size ) return;

  rawLine.clear();
  for ( int rawIP = ip; ip < size; rawIP++ ) {
    const unsigned char c = program.at( rawIP );
    if ( c == zztNewLine ) break;
    rawLine += c;
  }

  const unsigned char main_symbol = program.at( ip++ );
  // zdebug() << "SYMBOL" << &program << ip << (char) main_symbol;
  switch(main_symbol)
  {
    case zztNewLine: return;

    case '#': {
      comType = Command::Crunch;
      int loop = 0;
      while( (loop++) < 32 ) {
        ZString token;  
        getOneToken( program, ip, token, crunchDelimiters );
        if ( !token.empty() ) {
          zdebug() << __FILE__ << __LINE__ << token;
          tokens.push_back( token );
        }
        char delimiter = program.at(ip);
        ip += 1;
        if ( delimiter == zztNewLine ) break;
      }

      if (loop >= 32) { zerror() << "INFINITE LOOP" << __FILE__ << __LINE__; break; }
      break;
    }

    case '@': {
      comType = Command::Name;
      ZString token;
      getWholeLine( program, ip, token );
      zdebug() << __FILE__ << __LINE__ << token;
      tokens.push_back( token );
      ip += 1;
      break;
    }

    case ':': {
      comType = Command::Label;
      ZString token;
      getWholeLine( program, ip, token );
      tokens.push_back( token );
      ip += 1;
      break;
    }

    case '/': {
      comType = Command::Move;
      ZString token;
      getOneToken( program, ip, token, moveDelimiters );
      tokens.push_back( token );
      break;
    }

    case '?': {
      comType = Command::Try;
      ZString token;
      getOneToken( program, ip, token, moveDelimiters );
      tokens.push_back( token );
      break;
    }

    case '!': {
      comType = Command::Menu;
      ZString token;
      getOneToken( program, ip, token, menuDelimiters );
      tokens.push_back( token );
      if ( program.at( ip++ ) == zztNewLine ) {
        break;
      }
      getWholeLine( program, ip, token );
      tokens.push_back( token );
      break;
    }

    case '$': {
      comType = Command::PrettyText;
      ZString token;
      getWholeLine( program, ip, token );
      tokens.push_back( token );
      ip += 1;
      break;
    }

    case '\'': {
      comType = Command::Remark;
      ZString token;
      getWholeLine( program, ip, token );
      tokens.push_back( token );
      ip += 1;
      break;
    }

    case ' ':
    default: {
      comType = Command::Text;
      ip -= 1;
      ZString token;
      getWholeLine( program, ip, token );
      tokens.push_back( token );
      ip += 1;
      break;
    }
  }
}

struct TokenCommandPair {
  ZString token;
  Crunch::Code code;
};

static TokenCommandPair tokenCommandTable[] =
{
  { "BECOME",    Crunch::Become },
  { "BIND",      Crunch::Bind },
  { "CHANGE",    Crunch::Change },
  { "CHAR",      Crunch::Char },
  { "CLEAR",     Crunch::Clear },
  { "CYCLE",     Crunch::Cycle },
  { "DIE",       Crunch::Die },
  { "END",       Crunch::End },
  { "ENDGAME",   Crunch::Endgame },
  { "GIVE",      Crunch::Give },
  { "GO",        Crunch::Go },
  { "IDLE",      Crunch::Idle },
  { "IF",        Crunch::If },
  { "LOCK",      Crunch::Lock },
  { "PLAY",      Crunch::Play },
  { "PUT",       Crunch::Put },
  { "RESTART",   Crunch::Restart },
  { "RESTORE",   Crunch::Restore },
  { "SEND",      Crunch::Send },
  { "SET",       Crunch::Set },
  { "SHOOT",     Crunch::Shoot },
  { "TAKE",      Crunch::Take },
  { "THROWSTAR", Crunch::Throwstar },
  { "TRY",       Crunch::Try },
  { "UNLOCK",    Crunch::Unlock },
  { "WALK",      Crunch::Walk },
  { "ZAP",       Crunch::Zap },
  { "", Crunch::None }
};

static Crunch::Code tokenizeCrunch( const ZString &token )
{
  ZString capToken = token.upper();

  for ( int i = 0; tokenCommandTable[i].code != Crunch::None; i++ ) {
    if ( capToken == tokenCommandTable[i].token )
      return tokenCommandTable[i].code;
  }
  return Crunch::None;
}

static int executeCrunch( ScriptableThing *thing, Crunch::Code code,
                          list<ZString> &tokens, int cycles )
{
  switch ( code )
  {
    case Crunch::End:
      thing->setPaused( true );
      cycles = 0;
      break;

    case Crunch::Char:
      if ( verifyTokens( tokens, 2 ) ) {
        tokens.pop_front();
        int i = atoi( tokens.front().c_str() );
        thing->execChar( i );
        cycles -= 1;
      }
      else {
        /* */
      }
      break;

    case Crunch::Zap:
      if ( verifyTokens( tokens, 2 ) ) {
        tokens.pop_front();
        thing->execZap( tokens.front() );
        cycles -= 1;
      }
      else {
        /* */
      }
      break;

    case Crunch::Restore:
      if ( verifyTokens( tokens, 2 ) ) {
        tokens.pop_front();
        thing->execRestore( tokens.front() );
        cycles -= 1;
      }
      else {
        /* */
      }
      break;

    case Crunch::Play:
      if ( verifyTokens( tokens, 2 ) ) {
        tokens.pop_front();
        thing->execPlay( tokens.front() );
        cycles -= 1;
      }
      else {
        /* */
      }
      break;

    case Crunch::Restart:
      if ( verifyTokens( tokens, 1 ) ) {
        thing->execSend("RESTART");
        cycles -= 1;
      }
      else {
        /* */
      }
      break;

    case Crunch::Send:
    case Crunch::None:
      if ( tokens.size() == 1 ) {
        thing->execSend(tokens.front());
        cycles -= 1;
      }
      else if ( tokens.size() == 2 ) {
        ZString to = tokens.front();
        tokens.pop_front();
        ZString label = tokens.front();
        zdebug() << "#SEND" << to << label;
        thing->execSend(to, label);
        cycles -= 1;
      }
      else {
        /* */
      }
      break;

    default: {
      ZString err = "Invalid code:";
      for ( list<ZString>::iterator i = tokens.begin();
            i != tokens.end();
            i++ )
      {
        err.append( " " );
        err.append( *i );
      }
      throwError( thing, err );
      cycles = 0;
      break;
    }
  }

  return cycles;
}

static int showStrings( ScriptableThing *thing, const ProgramBank &program,
                        signed short &ip, int cycles )
{
  // Assumption: Empty lines have already been ignored.
  // pull two lines and decide if it's a message line or
  // a scroll. The first line is assumed to be text.

  signed short instructionPointer = ip;
  Command::Code firstComType;
  std::list<ZString> firstTokens;
  ZString firstRawLine;
  parseTokens( program, instructionPointer, firstComType, firstTokens, firstRawLine );
  const signed short afterFirstLineIP = instructionPointer;

  if ( ip >= length(program) ) {
    // end of program, so only one line of message.
    thing->showMessage( firstRawLine );
    ip = afterFirstLineIP;
    return cycles - 1;
  }

  Command::Code comType;
  std::list<ZString> tokens;
  ZString rawLine;
  parseTokens( program, instructionPointer, comType, tokens, rawLine );

  if ( comType == Command::Crunch ||
       comType == Command::Try ||
       comType == Command::Move ) {
    // only one string before new commands
    thing->showMessage( firstRawLine );
    ip = afterFirstLineIP;
    return cycles - 1;
  }

  // Definitely two strings, so make a model and add strings.
  // inefficient, rewind parsing and reparse strings.
  instructionPointer = ip;
  TextScrollModel *model = new TextScrollModel;

  while ( ip < length(program) )
  {
    bool validCom = true;
    signed short instructionPointer = ip;
    parseTokens( program, instructionPointer, comType, tokens, rawLine );

    switch ( comType ) {
      case Command::Text:
        model->appendPlainText( tokens.front() );
        break;
      case Command::PrettyText:
        model->appendPrettyText( tokens.front() );
        break;
      case Command::Menu: {
        ZString label = tokens.front();
        tokens.pop_front();
        ZString message = tokens.front();
        model->appendMenuText( label, message );
        break;
      }

      case Command::Crunch:
      case Command::Try:
      case Command::Move:
        validCom = false;

      default:
        model->appendPlainText( ZString() );
        break;
    }

    // non text type, bail out.
    if ( !validCom ) break;
    // advance parsing
    ip = instructionPointer;
  }

  thing->showScroll( model );
  return 0;
}

static bool seekToken( const ProgramBank &program,
                       Command::Code seekCom,
                       const ZString &label,
                       signed short &targetIP )
{
  // I wonder how slow this approach is.
  ZString capSeek = label.upper();

  // restart is an implied lable. seek it.
  if ( capSeek == "RESTART" ) {
    targetIP = 0;
    return true;
  }

  signed short ip = 0;
  while ( ip < length(program) )
  {
    Command::Code comType;
    list<ZString> tokens; 
    signed short instructionPointer = ip;
    ZString rawLine;
    parseTokens( program, instructionPointer, comType, tokens, rawLine );
    if ( comType == seekCom ) {
      ZString capLabel = tokens.front().upper();
      if ( capSeek.compare( capLabel ) == 0 ) {
        targetIP = ip;
        return true;
      }
    }
    ip = instructionPointer;
  }
  return false;
}

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
  Command::Code comType;
  list<ZString> tokens;
  signed short instructionPointer = 0;
  ZString rawLine;
  parseTokens( program, instructionPointer, comType, tokens, rawLine );

  if ( comType == Command::Name ) {
    return tokens.front();
  }

  return "";
}

void Interpreter::run( ScriptableThing *thing, int cycles )
{
  while ( cycles > 0 )
  {
    if ( thing->paused() ) break;

    signed short ip = thing->instructionPointer();
    signed short nextIP = ip;
    if ( ip >= length(program) ) break;

    Command::Code comType;
    list<ZString> tokens;
    ZString rawLine;

    parseTokens( program, nextIP, comType, tokens, rawLine );

    switch ( comType )
    {
      case Command::Name:
      case Command::Remark:
      case Command::Label:
        zdebug() << "Ignored Command Type" << rawLine;
        // Names, Remarks, and Labels are skipped during execution.
        break;

      case Command::Text:
        zdebug() << "Text Command Type" << rawLine;
        nextIP = ip;
        cycles = showStrings( thing, program, nextIP, cycles );
        break;

      case Command::PrettyText:
        zdebug() << "PrettyText Command Type" << rawLine;
        nextIP = ip;
        cycles = showStrings( thing, program, nextIP, cycles );
        break;

      case Command::Menu:
        zdebug() << "Menu Command Type" << rawLine;
        nextIP = ip;
        cycles = showStrings( thing, program, nextIP, cycles );
        break;

      case Command::Move:
        zdebug() << "Move Command Type" << rawLine;
        if ( !thing->execGo( Idle ) ) {
          nextIP = ip;
        }
        cycles = 0;
        break;

      case Command::Try:
        zdebug() << "Try Command Type" << rawLine;
        thing->execTry( Idle );
        cycles = 0;
        break;

      case Command::Crunch: {
        zdebug() << "Crunch Command Type" << rawLine;
        Crunch::Code code = tokenizeCrunch( tokens.front() );
        cycles = executeCrunch( thing, code, tokens, cycles );
        break;
      }

      default:
        zwarn() << "Unknown Command Type" << rawLine;
        break;
    }

    thing->setInstructionPointer( nextIP );
  }
}

void Interpreter::seekLabel( ScriptableThing *thing, const ZString &label )
{
  signed short ip = thing->instructionPointer();

  if ( seekToken( program, Command::Label, label, ip ) ) {
    thing->setInstructionPointer(ip);
    thing->setPaused( false );
  }
}

void Interpreter::zapLabel( const ZString &label )
{
  signed short ip = 0;
  if ( seekToken( program, Command::Label, label, ip ) ) {
    program[ip] = '\'';
  }
}

void Interpreter::restoreLabel( const ZString &label )
{
  signed short ip = 0;
  if ( seekToken( program, Command::Remark, label, ip ) ) {
    program[ip] = ':';
  }
}

