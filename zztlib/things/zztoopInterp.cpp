
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
#include "gameBoard.h"
#include "gameWorld.h"
#include "randomizer.h"
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

signed short seekForward( const ProgramBank &program, signed short ip, const ZString &delimiter )
{
  const int size = length(program);
  while ( ip < size ) {
    const unsigned char symbol = program.at( ip );
    if ( delimiter.find(symbol) != ZString::npos ) break;
    ip += 1;
  }
  return ip;
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
    ip = seekForward( program, ip, wholeLineDelimiter );
    if ( ip < size ) ip++;
  }
  return false;
}

// -------------------------------------

#if 0
/*
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


  mapper["ANY"] = Token::ANY;
  mapper["ALLIGNED"] = Token::ALIGNED; // aligned is mispelt in ZZT.exe
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
*/

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

#endif

// -------------------------------------

enum KILLENUM {
  FREEBIE,
  PROCEED,
  ENDCYCLE,
  COMMANDERROR
};

// -------------------------------------

class Runtime
{
  public:
    Runtime( ScriptableThing *pThing, ProgramBank &pProgram );
    ~Runtime();

    void run( int cycles );

  protected:
    KILLENUM parseNext();
    KILLENUM sugarMove();
    KILLENUM executeCrunch();

    void showStrings();
    void addString();
    void seekNextLine();
    void seekNextToken();

    void sendMessage( const ZString &mesg );

    ZString readLine();
    ZString getToken();

    KILLENUM execBecome();
    KILLENUM execBind();
    KILLENUM execChange();
    KILLENUM execChar();
    KILLENUM execClear();
    KILLENUM execCycle();
    KILLENUM execDie();
    KILLENUM execEnd();
    KILLENUM execEndgame();
    KILLENUM execGive();
    KILLENUM execGo();
    KILLENUM execIdle();
    KILLENUM execIf();
    KILLENUM execLock();
    KILLENUM execPlay();
    KILLENUM execPut();
    KILLENUM execRestart();
    KILLENUM execRestore();
    KILLENUM execSend();
    KILLENUM execSet();
    KILLENUM execShoot();
    KILLENUM execTake();
    KILLENUM execThrowstar();
    KILLENUM execTry();
    KILLENUM execUnlock();
    KILLENUM execWalk();
    KILLENUM execZap();

    KILLENUM throwError( const ZString &mesg );

  private:
    ScriptableThing *thing;
    ProgramBank &program;
    GameWorld *world;
    GameBoard *board;
    signed short ip;
    signed short startLineIP;
    int size;
    vector<ZString> displayLines;
};

// -------------------------------------

Runtime::Runtime( ScriptableThing *pThing, ProgramBank &pProgram )
  : thing( pThing ),
    program( pProgram )
{
  ip = thing->instructionPointer();
  board = thing->board();
  world = board->world();
  size = length(program);
}

Runtime::~Runtime()
{
  showStrings();
  thing->setInstructionPointer( ip );
}

void Runtime::run( int cycles )
{
  while ( cycles > 0 )
  {
    if ( thing->paused() ) break;

    if ( ip >= length(program) ) break;

    KILLENUM kill = parseNext();

    if ( kill == ENDCYCLE ) break;
    else if ( kill == PROCEED ) cycles --;
  }
}

void Runtime::seekNextLine()
{
  const ZString delimiters(wholeLineDelimiter);

  while ( ip < size ) {
    const unsigned char symbol = program.at( ip++ );
    if ( delimiters.find(symbol) != ZString::npos ) break;
  }
}

void Runtime::seekNextToken()
{
  while ( ip < size ) {
    const unsigned char symbol = program.at( ip++ );
    if ( symbol != ' ' ) break;
    ip++;
  }
}

ZString Runtime::readLine()
{
  const ZString delimiters(wholeLineDelimiter);

  ZString line;
  while ( ip < size ) {
    const unsigned char symbol = program.at( ip );
    if ( delimiters.find(symbol) != ZString::npos ) break;
    line.push_back(symbol);
    ip++;
  }
  ip++; // advance past newline symbol
  return line;
}

ZString Runtime::getToken()
{
  ZString token;
  bool foundNonSpace = false;
  while ( ip < size ) {
    const unsigned char symbol = program.at( ip );
    if ( symbol == zztNewLine ) break;
    if ( foundNonSpace && symbol == ' ' ) break;
    if ( !foundNonSpace && symbol != ' ' ) foundNonSpace = true;
    token.push_back(symbol);
    ip++;
  }
  return token;
}

KILLENUM Runtime::parseNext()
{
  if ( ip >= size ) return ENDCYCLE;

  startLineIP = ip;

  const unsigned char main_symbol = program.at( ip );
  KILLENUM ret = FREEBIE;
  switch ( main_symbol )
  {
    case '@':
    case '\'':
    case ':':
      zdebug() << __FILE__ << ":" << __LINE__ << " ignored type";
      break;

    case '#':
      zdebug() << __FILE__ << ":" << __LINE__ << " crunch";
      ret = executeCrunch();
      break;

    case '?':
    case '/': // currently unimplemented
      zdebug() << __FILE__ << ":" << __LINE__ << " sugar move";
      ret = sugarMove();
      break;

    case zztNewLine:
    case '!':
    case '$':
    case ' ':
    default:
      zdebug() << __FILE__ << ":" << __LINE__ << " strings";
      addString();
      break;
  }

  // Post line cleanup
  ip = startLineIP;
  seekNextLine();

  return FREEBIE;
}

void Runtime::addString()
{
  ZString line = readLine();
  if ( line.empty() && displayLines.size() < 1 ) return;
  displayLines.push_back(line);
}

void Runtime::showStrings()
{
  const int count = displayLines.size();

  // determine difference between Message and Scroll
  if ( count == 1 )
  {
    thing->showMessage( displayLines.front() );
  }
  else if ( count >= 2 )
  {
    TextScrollModel *model = new TextScrollModel;

    for ( int i = 0; i < count; i++ )
    {
      ZString line = displayLines.at(i);
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
  }
}

KILLENUM Runtime::sugarMove()
{
  const bool forcedMove = ( program.at(ip) == '/' );

  // Currently Unimplemented

  return ENDCYCLE;
}

KILLENUM Runtime::executeCrunch()
{
  ip++; // point after the crunch.

  if ( ip >= size ) return throwError( "END OF PROGRAM" );

  ZString token = getToken().upper();
  zdebug() << __FILE__ << ":" << __LINE__ << token;

  KILLENUM ret = PROCEED;

  if ( token == "BECOME" ) ret = execBecome();
  else if ( token == "BIND" ) ret = execBind();
  else if ( token == "CHANGE" ) ret = execChange();
  else if ( token == "CHAR" ) ret = execChar();
  else if ( token == "CLEAR" ) ret = execClear();
  else if ( token == "CYCLE" ) ret = execCycle();
  else if ( token == "DIE" ) ret = execDie();
  else if ( token == "END" ) ret = execEnd();
  else if ( token == "ENDGAME" ) ret = execEndgame();
  else if ( token == "GIVE" ) ret = execGive();
  else if ( token == "GO" ) ret = execGo();
  else if ( token == "IDLE" ) ret = execIdle();
  else if ( token == "IF" ) ret = execIf();
  else if ( token == "LOCK" ) ret = execLock();
  else if ( token == "PLAY" ) ret = execPlay();
  else if ( token == "PUT" ) ret = execPut();
  else if ( token == "RESTART" ) ret = execRestart();
  else if ( token == "RESTORE" ) ret = execRestore();
  else if ( token == "SEND" ) ret = execSend();
  else if ( token == "SET" ) ret = execSet();
  else if ( token == "SHOOT" ) ret = execShoot();
  else if ( token == "TAKE" ) ret = execTake();
  else if ( token == "THROWSTAR" ) ret = execThrowstar();
  else if ( token == "TRY" ) ret = execTry();
  else if ( token == "UNLOCK" ) ret = execUnlock();
  else if ( token == "WALK" ) ret = execWalk();
  else if ( token == "ZAP" ) ret = execZap();
  else {
    sendMessage( token );
  }

  return ret;
}

KILLENUM Runtime::execBecome()
{
  return PROCEED;
}

KILLENUM Runtime::execBind()
{
  return PROCEED;
}

KILLENUM Runtime::execChange()
{
  return PROCEED;
}

KILLENUM Runtime::execChar()
{
  return PROCEED;
}

KILLENUM Runtime::execClear()
{
  return PROCEED;
}

KILLENUM Runtime::execCycle()
{
  return PROCEED;
}

KILLENUM Runtime::execDie()
{
  return PROCEED;
}

KILLENUM Runtime::execEnd()
{
  thing->setPaused( true );
  return ENDCYCLE;
}

KILLENUM Runtime::execEndgame()
{
  return PROCEED;
}

KILLENUM Runtime::execGive()
{
  return PROCEED;
}

KILLENUM Runtime::execGo()
{
  return PROCEED;
}

KILLENUM Runtime::execIdle()
{
  return ENDCYCLE;
}

KILLENUM Runtime::execIf()
{
  return PROCEED;
}

KILLENUM Runtime::execLock()
{
  return PROCEED;
}

KILLENUM Runtime::execPlay()
{
  ZString song = readLine();
  thing->execPlay( song );
  return PROCEED;
}

KILLENUM Runtime::execPut()
{
  return PROCEED;
}

KILLENUM Runtime::execRestart()
{
  return PROCEED;
}

KILLENUM Runtime::execRestore()
{
  return PROCEED;
}

KILLENUM Runtime::execSend()
{
  seekNextToken();
  ZString mesg = readLine();
  sendMessage( mesg );
  return PROCEED;
}

KILLENUM Runtime::execSet()
{
  return PROCEED;
}

KILLENUM Runtime::execShoot()
{
  return PROCEED;
}

KILLENUM Runtime::execTake()
{
  return PROCEED;
}

KILLENUM Runtime::execThrowstar()
{
  return PROCEED;
}

KILLENUM Runtime::execTry()
{
  return PROCEED;
}

KILLENUM Runtime::execUnlock()
{
  return PROCEED;
}

KILLENUM Runtime::execWalk()
{
  return PROCEED;
}

KILLENUM Runtime::execZap()
{
  return PROCEED;
}

void Runtime::sendMessage( const ZString &mesg )
{
  size_t delim = mesg.find_first_of(':');

  if ( delim == string::npos ) {
    thing->seekLabel( mesg );
  }
  else {
    ZString to = mesg.substr( 0, delim-1 );
    ZString label = mesg.substr( delim+1 );
    board->sendLabel( to, label, thing );
  }
}

KILLENUM Runtime::throwError( const ZString &mesg )
{
  ip = startLineIP; // rewind to capture whole line
  ZString line = readLine();
  zinfo() << "ZZTOOP ERROR:" << mesg << "-->" << line;
  thing->setPaused(true);
  return ENDCYCLE;
}

// -------------------------------------

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
  ZZTOOP::Runtime runtime( thing, program );
  runtime.run( cycles );
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

