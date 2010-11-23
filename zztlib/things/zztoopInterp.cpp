
#include <cassert>
#include <list>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <memory>
#include <vector>
#include <map>

#include "debug.h"
#include "zstring.h"
#include "scriptable.h"
#include "textScrollModel.h"
#include "randomizer.h"
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
static const char tokenDelimiters[] = { ' ', zztNewLine, 0 };
static const char crunchDelimiters[] = { ' ', ':', zztNewLine, 0 };
static const char moveDelimiters[] = { ' ', '@', '#', ':', '/', '?', '!', '$', '\'', zztNewLine, 0 };
static const char menuDelimiters[] = { ' ', ';', zztNewLine, 0 };

ZString getOneToken( const ProgramBank &program,
                     signed short &ip,
                     const ZString &delimiter )
{
  ZString token;
  const int size = length(program);
  while ( ip < size ) {
    const unsigned char symbol = program.at( ip );
    if ( delimiter.find(symbol) != ZString::npos ) break;
    token.push_back( symbol );
    ip += 1;
  }
  return token;
}

ZString getWholeLine( const ProgramBank &program, signed short &ip )
{
  return getOneToken( program, ip, wholeLineDelimiter );
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
      ZString token = getOneToken( program, ip, tokenDelimiters );
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

namespace Token
{
  enum Code {
    no_token = 0,
    // flags
    ANY,
    ALIGNED,
    CONTACT,
    ENERGIZED,
    BLOCKED,
    NOT,
    // Items
    AMMO,
    TORCH,
    GEMS,
    HEALTH,
    TIME,
    SCORE,
    // Colors
    BLACK,
    BLUE,
    CYAN,
    GRAY,
    GREEN,
    PURPLE,
    RED,
    WHITE,
    YELLOW,
    // Crunch Commands
    BECOME,
    BIND,
    CHANGE,
    CHAR,
    CLEAR,
    CYCLE,
    DIE,
    END,
    ENDGAME,
    GIVE,
    GO,
    IDLE,
    IF,
    LOCK,
    PLAY,
    PUT,
    RESTART,
    RESTORE,
    SEND,
    SET,
    SHOOT,
    TAKE,
    THROWSTAR,
    TRY,
    UNLOCK,
    WALK,
    ZAP,
    // Directions
    NORTH,
    SOUTH,
    WEST,
    EAST,
    CLOCKWISE,
    COUNTERWISE,
    SEEK,
    FLOW,
    OPPOSITE,
    RANDNS,
    RANDNE,
    RANDP,
    // entities
    max_tokens
  };

  void InitTokenMap();
  Code translate( const ZString &s );
  
  typedef std::map<ZString, Code> ZStringCodeMap;
  ZStringCodeMap mapper;
};

void Token::InitTokenMap()
{
  if (!mapper.empty()) return;

  mapper["ANY"] = Token::ANY;
  mapper["ALIGNED"] = Token::ALIGNED;
  mapper["CONTACT"] = Token::CONTACT;
  mapper["ENERGIZED"] = Token::ENERGIZED;
  mapper["BLOCKED"] = Token::BLOCKED;
  mapper["NOT"] = Token::NOT;

  mapper["AMMO"] = Token::AMMO;
  mapper["TORCH"] = Token::TORCH;
  mapper["GEMS"] = Token::GEMS;
  mapper["HEALTH"] = Token::HEALTH;
  mapper["TIME"] = Token::TIME;
  mapper["SCORE"] = Token::SCORE;

  mapper["BLACK"] = Token::BLACK;
  mapper["BLUE"] = Token::BLUE;
  mapper["CYAN"] = Token::CYAN;
  mapper["GRAY"] = Token::GRAY;
  mapper["GREEN"] = Token::GREEN;
  mapper["PURPLE"] = Token::PURPLE;
  mapper["RED"] = Token::RED;
  mapper["WHITE"] = Token::WHITE;
  mapper["YELLOW"] = Token::YELLOW;

  mapper["BECOME"] = Token::BECOME;
  mapper["BIND"] = Token::BIND;
  mapper["CHANGE"] = Token::CHANGE;
  mapper["CHAR"] = Token::CHAR;
  mapper["CLEAR"] = Token::CLEAR;
  mapper["CYCLE"] = Token::CYCLE;
  mapper["DIE"] = Token::DIE;
  mapper["END"] = Token::END;
  mapper["ENDGAME"] = Token::ENDGAME;
  mapper["GIVE"] = Token::GIVE;
  mapper["GO"] = Token::GO;
  mapper["IDLE"] = Token::IDLE;
  mapper["IF"] = Token::IF;
  mapper["LOCK"] = Token::LOCK;
  mapper["PLAY"] = Token::PLAY;
  mapper["PUT"] = Token::PUT;
  mapper["RESTART"] = Token::RESTART;
  mapper["RESTORE"] = Token::RESTORE;
  mapper["SEND"] = Token::SEND;
  mapper["SET"] = Token::SET;
  mapper["SHOOT"] = Token::SHOOT;
  mapper["TAKE"] = Token::TAKE;
  mapper["THROWSTAR"] = Token::THROWSTAR;
  mapper["TRY"] = Token::TRY;
  mapper["UNLOCK"] = Token::UNLOCK;
  mapper["WALK"] = Token::WALK;
  mapper["ZAP"] = Token::ZAP;

  mapper["N"] = Token::NORTH;
  mapper["NORTH"] = Token::NORTH;
  mapper["S"] = Token::SOUTH;
  mapper["SOUTH"] = Token::SOUTH;
  mapper["W"] = Token::WEST;
  mapper["WEST"] = Token::WEST;
  mapper["E"] = Token::EAST;
  mapper["EAST"] = Token::EAST;
  mapper["CLOCKWISE"] = Token::CLOCKWISE;
  mapper["COUNTERWISE"] = Token::COUNTERWISE;
  mapper["SEEK"] = Token::SEEK;
  mapper["FLOW"] = Token::FLOW;
  mapper["OPPOSITE"] = Token::OPPOSITE;
  mapper["RANDNS"] = Token::RANDNS;
  mapper["RANDNE"] = Token::RANDNE;
  mapper["RANDP"] = Token::RANDP;
}

Token::Code Token::translate( const ZString &s )
{
  InitTokenMap();

  const Token::ZStringCodeMap::const_iterator iter = mapper.find(s);
  if ( iter == mapper.end() ) {
    return Token::no_token;
  }

  return iter->second;
}

// -------------------------------------

static const int DIRECTION_ERROR = -1;

int cardinal_clockwise( int dir )
{
  switch ( dir ) {
    case ZZTThing::North: return ZZTThing::East; break;
    case ZZTThing::South: return ZZTThing::West; break;
    case ZZTThing::West: return ZZTThing::North; break;
    case ZZTThing::East: return ZZTThing::South; break;
    default: break;
  }
  return DIRECTION_ERROR;
}

int cardinal_counterwise( int dir )
{
  switch ( dir ) {
    case ZZTThing::North: return ZZTThing::West; break;
    case ZZTThing::South: return ZZTThing::East; break;
    case ZZTThing::West: return ZZTThing::South; break;
    case ZZTThing::East: return ZZTThing::North; break;
    default: break;
  }
  return DIRECTION_ERROR;
}

int cardinal_opposite( int dir )
{
  switch ( dir ) {
    case ZZTThing::North: return ZZTThing::South; break;
    case ZZTThing::South: return ZZTThing::North; break;
    case ZZTThing::West: return ZZTThing::West; break;
    case ZZTThing::East: return ZZTThing::East; break;
    default: break;
  }
  return DIRECTION_ERROR;
}

int cardinal_randp( int dir )
{
  int r = Randomizer::randomRange(2);
  switch ( dir ) {
    case ZZTThing::North:
    case ZZTThing::South: return r ? ZZTThing::West : ZZTThing::East; break;
    case ZZTThing::West:
    case ZZTThing::East: return r ? ZZTThing::North : ZZTThing::South; break;
    default: break;
  }
  return DIRECTION_ERROR;
}

int cardinal_randns()
{
  int r = Randomizer::randomRange(2);
  return r ? ZZTThing::North : ZZTThing::South;
}

int cardinal_randne()
{
  int r = Randomizer::randomRange(2);
  return r ? ZZTThing::North : ZZTThing::East;
}

int parseDirection( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  list<Token::Code> tokens;
  while ( program.at(ip) != zztNewLine ) {
    ZString token = getOneToken( program, ip, tokenDelimiters );
    Token::Code tokenCode = Token::translate( token );
    tokens.push_front( tokenCode );
  }

  int dir = 0;
  while ( !tokens.empty() ) {
    Token::Code tokenCode = tokens.front();
    tokens.pop_front();
    switch ( tokenCode ) {
      case Token::IDLE: dir = ZZTThing::Idle; break;
      case Token::NORTH: dir = ZZTThing::North; break;
      case Token::SOUTH: dir = ZZTThing::South; break;
      case Token::WEST: dir = ZZTThing::West; break;
      case Token::EAST: dir = ZZTThing::East; break;
      case Token::SEEK: dir = thing->seekDir(); break;
      case Token::FLOW: break;
      case Token::CLOCKWISE: dir = cardinal_clockwise(dir); break;
      case Token::COUNTERWISE: dir = cardinal_counterwise(dir); break;
      case Token::OPPOSITE: dir = cardinal_opposite(dir); break;
      case Token::RANDNS: dir = cardinal_randns(); break;
      case Token::RANDNE: dir = cardinal_randne(); break;
      case Token::RANDP: dir = cardinal_randp(dir); break;
      default: return DIRECTION_ERROR;
    }
  }

  return dir;
}

// -------------------------------------

enum KILLENUM {
  FREEBIE,
  PROCEED,
  ENDCYCLE,
  COMMANDERROR
};

KILLENUM zztoopError( ScriptableThing *thing, const ProgramBank &program,
                      signed short ip, const ZString &mesg )
{
  ZString rawLine = getWholeLine( program, ip );
  zinfo() << "ZZTOOP ERROR:" << mesg << " --> " << rawLine;
  thing->setPaused(true);
  return ENDCYCLE;
}

// -------------------------------------

KILLENUM showStrings( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  const int size = length(program);

  list<ZString> lines;
  int count = 0;

  // Collect strings
  while ( true )
  {
    ZString line = getWholeLine( program, ip );
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

// -------------------------------------

KILLENUM sugarMove( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  const bool forcedMove = ( program.at(ip) == '/' );

  // Currently Unimplemented

  return ENDCYCLE;
}

// -------------------------------------

namespace Crunch {

typedef KILLENUM (*ExecFunc)( ScriptableThing *thing, const ProgramBank &program, signed short &ip );

KILLENUM execBecome( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  return PROCEED;
}

KILLENUM execBind( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  return PROCEED;
}

KILLENUM execChange( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  return PROCEED;
}

KILLENUM execChar( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  ZString token = getOneToken( program, ip, tokenDelimiters );
  bool error = false;
  unsigned char ch = token.byte(&error);
  if (error) return COMMANDERROR;
  thing->execChar( ch );
  return PROCEED;
}

KILLENUM execClear( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  return PROCEED;
}

KILLENUM execCycle( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  return PROCEED;
}

KILLENUM execDie( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  thing->execDie();
  return PROCEED;
}

KILLENUM execEnd( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  thing->execEnd();
  return ENDCYCLE;
}

KILLENUM execEndgame( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  return PROCEED;
}

KILLENUM execGive( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  return PROCEED;
}

KILLENUM execGo( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  return PROCEED;
}

KILLENUM execIdle( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  return PROCEED;
}

KILLENUM execIf( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  return PROCEED;
}

KILLENUM execLock( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  return PROCEED;
}

KILLENUM execPlay( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  ZString song = getWholeLine( program, ip );
  thing->execPlay( song );
  return PROCEED;
}

KILLENUM execPut( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  return PROCEED;
}

KILLENUM execRestart( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  return PROCEED;
}

KILLENUM execRestore( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  return PROCEED;
}

KILLENUM execSend( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  return PROCEED;
}

KILLENUM execSet( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  return PROCEED;
}

KILLENUM execShoot( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  int dir = parseDirection( thing, program, ip );
  if ( dir == DIRECTION_ERROR ) {
    return COMMANDERROR;
  }
  thing->execShoot( dir );
  return PROCEED;
}

KILLENUM execTake( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  return PROCEED;
}

KILLENUM execThrowstar( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  int dir = parseDirection( thing, program, ip );
  if ( dir == DIRECTION_ERROR ) {
    return COMMANDERROR;
  }
  thing->execThrowstar( dir );
  return PROCEED;
}

KILLENUM execTry( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  return PROCEED;
}

KILLENUM execUnlock( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  return PROCEED;
}

KILLENUM execWalk( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  return PROCEED;
}

KILLENUM execZap( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  ZString label = getWholeLine( program, ip );
  thing->execZap( label );
  return PROCEED;
}

}; // namespace Crunch

KILLENUM executeCrunch( ScriptableThing *thing, const ProgramBank &program, signed short &ip )
{
  const int size = length(program);
  const signed short startLineIP = ip;
  ip++; // point after the crunch.

  if ( ip >= size ) {
    return zztoopError( thing, program, startLineIP, "Size Error!" );
  }

  // get crunch command
  ZString token = getOneToken( program, ip, crunchDelimiters ).upper();
  Token::Code tokenCode = Token::translate(token);
  zdebug() << __FILE__ << ":" << __LINE__ << " --> " << tokenCode << " " << token;

  // scan crunch commands and execute
  KILLENUM ret = PROCEED;
  Crunch::ExecFunc exec = 0;
  bool spc = true;

  switch ( tokenCode ) {
    case Token::BECOME: exec = &Crunch::execBecome; break;
    case Token::BIND: exec = &Crunch::execBind; break;
    case Token::CHANGE: exec = &Crunch::execChange; break;
    case Token::CHAR: exec = &Crunch::execChar; break;
    case Token::CLEAR: exec = &Crunch::execClear; break;
    case Token::CYCLE: exec = &Crunch::execCycle; break;
    case Token::DIE: exec = &Crunch::execDie; spc = false; break;
    case Token::END: exec = &Crunch::execEnd; spc = false; break;
    case Token::ENDGAME: exec = &Crunch::execEndgame; spc = false; break;
    case Token::GIVE: exec = &Crunch::execGive; break;
    case Token::GO: exec = &Crunch::execGo; break;
    case Token::IDLE: exec = &Crunch::execIdle; break;
    case Token::IF: exec = &Crunch::execIf; spc = false; break;
    case Token::LOCK: exec = &Crunch::execLock; break;
    case Token::PLAY: exec = &Crunch::execPlay; break;
    case Token::PUT: exec = &Crunch::execPut; break;
    case Token::RESTART: exec = &Crunch::execRestart; spc = false; break;
    case Token::RESTORE: exec = &Crunch::execRestore; break;
    case Token::SEND: exec = &Crunch::execSend; break;
    case Token::SET: exec = &Crunch::execSet; break;
    case Token::SHOOT: exec = &Crunch::execShoot; break;
    case Token::TAKE: exec = &Crunch::execTake; break;
    case Token::THROWSTAR: exec = &Crunch::execThrowstar; break;
    case Token::TRY: exec = &Crunch::execTry; break;
    case Token::UNLOCK: exec = &Crunch::execUnlock; break;
    case Token::WALK: exec = &Crunch::execWalk; break;
    case Token::ZAP: exec = &Crunch::execZap; break;
    default: break;
  }

  if (exec)
  {
    if (spc) {
      const bool space = ( program.at(ip) == ' ' );
      ip++;
      if ( !space || ip >= size ) {
        return zztoopError( thing, program, startLineIP, "Space Error!" );
      }
    }
      
    ret = exec( thing, program, ip );

    if ( ret == COMMANDERROR ) {
      return zztoopError( thing, program, startLineIP, "Command Error!" );
    }
  }
  else {
    // DEFAULT SEND COMMAND
  }

  // point past the newline.
  if ( ip < size && program.at(ip) == zztNewLine ) ip++;

  return ret;
}

// -------------------------------------

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
  return getWholeLine( program, ip );
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

