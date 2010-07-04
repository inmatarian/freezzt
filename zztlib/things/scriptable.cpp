/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <cassert>
#include <list>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <memory>

#include "debug.h"
#include "zstring.h"
#include "zztEntity.h"
#include "gameWorld.h"
#include "gameBoard.h"
#include "abstractMusicStream.h"
#include "abstractPainter.h"
#include "scrollView.h"
#include "textScrollModel.h"

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

void ScriptableThing::setProgram( ProgramBank *program  )
{
  m_program = program;

  ZZTOOP::Command comType;
  std::list<ZString> tokens;
  signed short instructionPointer = 0;
  ZString rawLine;
  parseTokens( *m_program, instructionPointer, comType, tokens, rawLine );
  if ( comType == ZZTOOP::Name &&
       verifyTokens( tokens, 1 ) )
  {
    setObjectName( tokens.front() );
  }
}

ProgramBank & ScriptableThing::program()
{
  assert( m_program );
  return *m_program;
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
static const char crunchDelimiters[] = { ' ', ':', zztNewLine, 0 };
static const char moveDelimiters[] = { ' ', '@', '#', ':', '/', '?', '!', '$', '\'', zztNewLine, 0 };
static const char menuDelimiters[] = { ' ', ';', zztNewLine, 0 };

static void getOneToken( const ProgramBank &program,
                         signed short &ip,
                         ZString &token,
                         const ZString &delimiter )
{
  token.clear();
  const int size = program.length();
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
                                   std::list<ZString> &tokens,
                                   ZString &rawLine )
{
  using namespace ZZTOOP;
  const int size = program.length();
  tokens.clear();
  comType = None;
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
      comType = ZZTOOP::Crunch;
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
      if ( program.at( ip++ ) == zztNewLine ) {
        break;
      }
      getWholeLine( program, ip, token );
      tokens.push_back( token );
      break;
    }

    case '$': {
      comType = PrettyText;
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

Crunch::Code ScriptableThing::tokenizeCrunch( const ZString &token )
{
  ZString capToken = token.upper();

  for ( int i = 0; tokenCommandTable[i].code != Crunch::None; i++ ) {
    if ( capToken == tokenCommandTable[i].token )
      return tokenCommandTable[i].code;
  }
  return Crunch::None;
}

int ScriptableThing::executeCrunch( Crunch::Code code,
                                    std::list<ZString> &tokens,
                                    int cycles )
{
  switch ( code )
  {
    case Crunch::End:
      setPaused( true );
      cycles = 0;
      break;

    case Crunch::Char:
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
      if ( verifyTokens( tokens, 2 ) ) {
        tokens.pop_front();
        playSong( tokens.front() );
        cycles -= 1;
      }
      else {
        /* */
      }
      break;

    case Crunch::Restart:
      if ( verifyTokens( tokens, 1 ) ) {
        seekLabel("RESTART");
        cycles -= 1;
      }
      else {
        /* */
      }
      break;

    case Crunch::Send:
    case Crunch::None:
      if ( tokens.size() == 1 ) {
        seekLabel(tokens.front());
        cycles -= 1;
      }
      else if ( tokens.size() == 2 ) {
        ZString to = tokens.front();
        tokens.pop_front();
        ZString label = tokens.front();
        zdebug() << "#SEND" << to << label;
        sendLabel(to, label);
        cycles -= 1;
      }
      else {
        /* */
      }
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

  return cycles;
}

void ScriptableThing::run( int cycles )
{
  if ( paused() ) return;

  while ( cycles > 0 )
  {
    if ( m_ip >= program().length() ) break;

    ZZTOOP::Command comType;
    std::list<ZString> tokens;
    signed short instructionPointer = m_ip;
    ZString rawLine;
    parseTokens( program(), instructionPointer, comType, tokens, rawLine );

    switch ( comType )
    {
      case ZZTOOP::Name:
      case ZZTOOP::Remark:
      case ZZTOOP::Label:
        zdebug() << "Ignored Command Type" << rawLine;
        // Names, Remarks, and Labels are skipped during execution.
        break;

      case ZZTOOP::Text:
        zdebug() << "Text Command Type" << rawLine;
        instructionPointer = m_ip;
        cycles = showStrings( program(), instructionPointer, cycles );
        break;

      case ZZTOOP::PrettyText:
        zdebug() << "PrettyText Command Type" << rawLine;
        instructionPointer = m_ip;
        cycles = showStrings( program(), instructionPointer, cycles );
        break;

      case ZZTOOP::Menu:
        zdebug() << "Menu Command Type" << rawLine;
        instructionPointer = m_ip;
        cycles = showStrings( program(), instructionPointer, cycles );
        break;

      case ZZTOOP::Move:
        zdebug() << "Move Command Type" << rawLine;
        if ( !execMove( Idle ) ) {
          instructionPointer = m_ip;
        }
        cycles = 0;
        break;

      case ZZTOOP::Try:
        zdebug() << "Try Command Type" << rawLine;
        execTry( Idle );
        cycles = 0;
        break;

      case ZZTOOP::Crunch: {
        zdebug() << "Crunch Command Type" << rawLine;
        Crunch::Code code = tokenizeCrunch( tokens.front() );
        cycles = executeCrunch( code, tokens, cycles );
        break;
      }

      default:
        zwarn() << "Unknown Command Type" << rawLine;
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

  // restart is an implied lable. seek it.
  if ( capSeek == "RESTART" ) {
    targetIP = 0;
    return true;
  }

  signed short ip = 0;
  while ( ip < program.length() )
  {
    ZZTOOP::Command comType;
    std::list<ZString> tokens; 
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

void ScriptableThing::seekLabel( const ZString &label )
{
  signed short ip = 0;
  if ( seekToken( program(), ZZTOOP::Label, label, ip ) ) {
    m_ip = ip;
    setPaused( false );
  }
}

void ScriptableThing::sendLabel( const ZString &to, const ZString &label )
{
  board()->sendLabel( to, label, this );
}

void ScriptableThing::zapLabel( const ZString &label )
{
  signed short ip = 0;
  if ( seekToken( program(), ZZTOOP::Label, label, ip ) ) {
    program()[ip] = '\'';
  }
}

void ScriptableThing::restoreLabel( const ZString &label )
{
  signed short ip = 0;
  if ( seekToken( program(), ZZTOOP::Remark, label, ip ) ) {
    program()[ip] = ':';
  }
}

void ScriptableThing::playSong( const ZString &label )
{
  musicStream()->playMusic( label.c_str() );
}

int ScriptableThing::showStrings( const ProgramBank &program,
                                  signed short &ip, int cycles )
{
  // Assumption: Empty lines have already been ignored.
  // pull two lines and decide if it's a message line or
  // a scroll. The first line is assumed to be text.

  signed short instructionPointer = ip;
  ZZTOOP::Command firstComType;
  std::list<ZString> firstTokens;
  ZString firstRawLine;
  parseTokens( program, instructionPointer, firstComType, firstTokens, firstRawLine );
  const signed short afterFirstLineIP = instructionPointer;

  if ( ip >= program.length() ) {
    // end of program, so only one line of message.
    board()->setMessage( firstRawLine );
    ip = afterFirstLineIP;
    return cycles - 1;
  }

  ZZTOOP::Command comType;
  std::list<ZString> tokens;
  ZString rawLine;
  parseTokens( program, instructionPointer, comType, tokens, rawLine );

  if ( comType == ZZTOOP::Crunch ||
       comType == ZZTOOP::Try ||
       comType == ZZTOOP::Move ) {
    // only one string before new commands
    board()->setMessage( firstRawLine );
    ip = afterFirstLineIP;
    return cycles - 1;
  }

  // Definitely two strings, so make a model and add strings.
  // inefficient, rewind parsing and reparse strings.
  instructionPointer = ip;
  TextScrollModel *model = new TextScrollModel;

  while ( ip < program.length() )
  {
    bool validCom = true;
    signed short instructionPointer = ip;
    parseTokens( program, instructionPointer, comType, tokens, rawLine );

    switch ( comType ) {
      case ZZTOOP::Text:
        model->appendPlainText( tokens.front() );
        break;
      case ZZTOOP::PrettyText:
        model->appendPrettyText( tokens.front() );
        break;
      case ZZTOOP::Menu: {
        ZString label = tokens.front();
        tokens.pop_front();
        ZString message = tokens.front();
        model->appendMenuText( label, message );
        break;
      }

      case ZZTOOP::Crunch:
      case ZZTOOP::Try:
      case ZZTOOP::Move:
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

  world()->scrollView()->setModel( model );
  return 0;
}

