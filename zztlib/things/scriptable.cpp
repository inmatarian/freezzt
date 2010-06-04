/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <list>
#include <string>
#include <algorithm>
#include <cstdlib>

#include "debug.h"
#include "zstring.h"
#include "zztEntity.h"
#include "gameWorld.h"
#include "gameBoard.h"
#include "abstractMusicStream.h"
#include "abstractPainter.h"

#include "zztThing.h"
#include "scriptable.h"

using namespace ZZTThing;

ScriptableThing::ScriptableThing()
  : m_ip(0),
    m_paused(true)
{
  /* */
}

inline bool verifyTokens( const std::list<ZString> &tokens,
                          unsigned int min, unsigned int max = 0 )
{
  if ( (max > 0) && (tokens.size()>max) ) return false;
  return ( tokens.size() >= min );
}

void ScriptableThing::setProgram( const ProgramBank &program  )
{
  m_program = program;

  ZZTOOP::Command comType;
  std::list<ZString> tokens;
  signed short instructionPointer = 0;
  parseTokens( m_program, instructionPointer, comType, tokens );
  if ( comType == ZZTOOP::Name &&
       verifyTokens( tokens, 1 ) )
  {
    setObjectName( tokens.front() );
  }

#if 1
  zout() << "\n";
  for ( unsigned int i = 0; i<program.size(); i++ ) {
    signed char c = program[i];
    if ( c == 0x0d ) zout() << " \\n ";
    else if ( c >= ' ' ) zout() << ZString(1, c);
    else zout() << "?";
  }

  zout() << "\n";
#endif
}

void ScriptableThing::setObjectName( const ZString &name )
{
  zinfo() << "Object named:" << name;
  m_name = name;
}

const ZString & ScriptableThing::objectName() const
{
  return m_name;
}

void ScriptableThing::setInstructionPointer( signed short ip )
{
  m_ip = (unsigned int) ip;
  // TODO: Handle objects with an ip of -1 (binding another robot)
}

// zzt's file format uses MSDOS 0x0d as the newline;
static const int zztNewLine = 0x0d;
static const char wholeLineDelimiter[] = { zztNewLine, 0 };
static const char crunchDelimiters[] = { ' ', zztNewLine, 0 };
static const char moveDelimiters[] = { ' ', '@', '#', ':', '/', '?', '!', '$', '\'', zztNewLine, 0 };
static const char menuDelimiters[] = { ' ', ':', zztNewLine, 0 };

static void getOneToken( const ProgramBank &program,
                         signed short &ip,
                         ZString &token,
                         const ZString &delimiter )
{
  token.clear();
  const int size = program.size();
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

void ScriptableThing::parseTokens( const ProgramBank &program,
                                   signed short &ip,
                                   ZZTOOP::Command &comType,
                                   std::list<ZString> &tokens )
{
  using namespace ZZTOOP;
  const int size = program.size();
  tokens.clear();
  comType = None;
  if ( ip > size ) return;

  const unsigned char main_symbol = program.at( ip++ );
  // zdebug() << "SYMBOL" << &program << ip << (char) main_symbol;
  switch(main_symbol)
  {
    case zztNewLine: return;

    case '#': {
      comType = ZZTOOP::Crunch;
      int loop = 0;
      ZString line;
      signed short otherIP = ip;
      getWholeLine( program, otherIP, line );
      // zdebug() << "WHOLE LINE" << line;
      while( (loop++) < 32 ) {
        ZString token;  
        getOneToken( program, ip, token, crunchDelimiters );
        if ( !token.empty() ) {
          zdebug() << __LINE__ << token;
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
      comType = Name;
      ZString token;
      getWholeLine( program, ip, token );
      zdebug() << __LINE__ << token;
      tokens.push_back( token );
      ip += 1;
      break;
    }

    case ':': {
      comType = Label;
      ZString token;
      getWholeLine( program, ip, token );
      tokens.push_back( token );
      ip += 1;
      break;
    }

    case '/': {
      comType = Move;
      ZString token;
      getOneToken( program, ip, token, moveDelimiters );
      tokens.push_back( token );
      break;
    }

    case '?': {
      comType = Try;
      ZString token;
      getOneToken( program, ip, token, moveDelimiters );
      tokens.push_back( token );
      break;
    }

    case '!': {
      comType = Menu;
      ZString token;
      getOneToken( program, ip, token, menuDelimiters );
      tokens.push_back( token );
      if ( program.at( ip ) == zztNewLine ) {
        ip += 1;
        break;
      }
      getWholeLine( program, ip, token );
      tokens.push_back( token );
      break;
    }

    case '$': {
      comType = Text;
      ZString token;
      getWholeLine( program, ip, token );
      tokens.push_back( token );
      ip += 1;
      break;
    }

    case '\'': {
      comType = Remark;
      ZString token;
      getWholeLine( program, ip, token );
      tokens.push_back( token );
      ip += 1;
      break;
    }

    case ' ':
    default: {
      comType = Text;
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

Crunch::Code ScriptableThing::tokenizeCrunch( const ZString &token )
{
  ZString capToken = token.upper();

  for ( int i = 0; tokenCommandTable[i].code != Crunch::None; i++ ) {
    if ( capToken == tokenCommandTable[i].token )
      return tokenCommandTable[i].code;
  }
  return Crunch::None;
}

void ScriptableThing::run( int cycles )
{
  if ( paused() ) return;

  while ( cycles > 0 )
  {
    if ( m_ip >= m_program.size() ) break;

    ZZTOOP::Command comType;
    std::list<ZString> tokens;
    signed short instructionPointer = m_ip;
    parseTokens( m_program, instructionPointer, comType, tokens );

    switch ( comType )
    {
      case ZZTOOP::Name:
      case ZZTOOP::Remark:
      case ZZTOOP::Label:
        zdebug() << "Ignored Command Type";
        // Names, Remarks, and Labels are skipped during execution.
        break;

      case ZZTOOP::Text:
        zdebug() << "Text Command Type";
        // Start and Load up a ScrollModel with Text
        break;

      case ZZTOOP::PrettyText:
        zdebug() << "PrettyText Command Type";
        // Start and Load up a ScrollModel with Pretty text
        break;

      case ZZTOOP::Menu:
        zdebug() << "Menu Command Type";
        // Start and Load up a ScrollModel with a Menu
        break;

      case ZZTOOP::Move:
        if ( !execMove( Idle ) ) {
          instructionPointer = m_ip;
        }
        cycles = 0;
        break;

      case ZZTOOP::Try:
        execTry( Idle );
        cycles = 0;
        break;

      case ZZTOOP::Crunch: {
        Crunch::Code code = tokenizeCrunch( tokens.front() );
        switch ( code ) {
          case Crunch::End:
            zdebug() << "#end";
            setPaused( true );
            cycles = 0;
            break;

          case Crunch::Char:
            zdebug() << "#char";
            if ( verifyTokens( tokens, 2 ) ) {
              tokens.pop_front();
              int i = atoi( tokens.front().c_str() );
              setCharacter( i );
              cycles -= 1;
            }
            else {
              /* */
            }
            break;

          case Crunch::Zap:
            zdebug() << "#zap";
            if ( verifyTokens( tokens, 2 ) ) {
              tokens.pop_front();
              zapLabel( tokens.front() );
              cycles -= 1;
            }
            else {
              /* */
            }
            break;

          case Crunch::Restore:
            zdebug() << "#restore";
            if ( verifyTokens( tokens, 2 ) ) {
              tokens.pop_front();
              restoreLabel( tokens.front() );
              cycles -= 1;
            }
            else {
              /* */
            }
            break;

          case Crunch::Play:
            zdebug() << "#play";
            if ( verifyTokens( tokens, 2 ) ) {
              tokens.pop_front();
              playSong( tokens.front() );
              cycles -= 1;
            }
            else {
              /* */
            }
            break;
  
          case Crunch::Send:
          case Crunch::Restart:
          case Crunch::None:
            zdebug() << "#send";
            cycles -= 1;
            break;

          default: {
            ZString err = "Invalid code:";
            for ( std::list<ZString>::iterator i = tokens.begin();
                  i != tokens.end();
                  i++ )
            {
              err.append( " " );
              err.append( *i );
            }
            throwError( err );
            cycles = 0;
            break;
          }
        }
        break;
      }

      default:
        break;
    }

    m_ip = instructionPointer;
  }

  // Pass the scrollmodel up to the world for showing.
}

void ScriptableThing::throwError( const ZString &text )
{
  zinfo() << "ZZTOOP ERROR:" << text;
  setPaused(true);
}

bool ScriptableThing::seekToken( const ProgramBank &program,
                                 ZZTOOP::Command seekCom,
                                 const ZString &label,
                                 signed short &targetIP )
{
  // I wonder how slow this approach is.
  ZString capSeek = label.upper();

  signed short ip = 0;
  while ( (unsigned) ip < program.size() )
  {
    ZZTOOP::Command comType;
    std::list<ZString> tokens; 
    signed short instructionPointer = ip;
    parseTokens( program, instructionPointer, comType, tokens );
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

void ScriptableThing::seekLabel( const ZString &label )
{
  signed short ip = 0;
  if ( seekToken( m_program, ZZTOOP::Label, label, ip ) ) {
    m_ip = ip;
    setPaused( false );
  }
}

void ScriptableThing::zapLabel( const ZString &label )
{
  signed short ip = 0;
  if ( seekToken( m_program, ZZTOOP::Label, label, ip ) ) {
    m_program[ip] = '\'';
  }
}

void ScriptableThing::restoreLabel( const ZString &label )
{
  signed short ip = 0;
  if ( seekToken( m_program, ZZTOOP::Remark, label, ip ) ) {
    m_program[ip] = ':';
  }
}

void ScriptableThing::playSong( const ZString &label )
{
  musicStream()->playMusic( label.c_str() );
}

