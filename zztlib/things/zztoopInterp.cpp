
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

namespace Token
{
  enum Code {
    UNKNOWN = 0,
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
    COUNTER,
    SEEK,
    FLOW,
    OPPOSITE,
    RANDNS,
    RANDNE,
    RANDP,
    // entities
    BEAR,
    BLINKWALL,
    BOMB,
    BOULDER,
    BULLET,
    BREAKABLE,
    DOOR,
    DUPLICATOR,
    EMPTY,
    ENERGIZER,
    FAKE,
    FOREST,
    GEM,
    HEAD,
    INVISIBLE,
    KEY,
    LINE,
    LION,
    MONITOR,
    NORMAL,
    OBJECT,
    PASSAGE,
    PLAYER,
    PUSHER,
    RICOCHET,
    RUFFIAN,
    SCROLL,
    SEGMENT,
    SHARK,
    SLIDEREW,
    SLIDERNS,
    SLIME,
    SOLID,
    SPINNINGGUN,
    STAR,
    TIGER,
    TRANSPORTER,
    WATER,
    max_tokens
  };
};

static Token::Code tokenize( const ZString &token )
{
  typedef map<ZString, Token::Code> TokenMapper;
  static TokenMapper mapper;

  if ( mapper.empty() ) 
  {
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
    mapper["COUNTER"] = Token::COUNTER;
    mapper["SEEK"] = Token::SEEK;
    mapper["FLOW"] = Token::FLOW;
    mapper["OPPOSITE"] = Token::OPPOSITE;
    mapper["RANDNS"] = Token::RANDNS;
    mapper["RANDNE"] = Token::RANDNE;
    mapper["RANDP"] = Token::RANDP;

    mapper["BEAR"] == Token::BEAR;
    mapper["BLINKWALL"] == Token::BLINKWALL;
    mapper["BOMB"] == Token::BOMB;
    mapper["BOULDER"] == Token::BOULDER;
    mapper["BULLET"] == Token::BULLET;
    mapper["BREAKABLE"] == Token::BREAKABLE;
    mapper["DOOR"] == Token::DOOR;
    mapper["DUPLICATOR"] == Token::DUPLICATOR;
    mapper["EMPTY"] == Token::EMPTY;
    mapper["ENERGIZER"] == Token::ENERGIZER;
    mapper["FAKE"] == Token::FAKE;
    mapper["FOREST"] == Token::FOREST;
    mapper["GEM"] == Token::GEM;
    mapper["HEAD"] == Token::HEAD;
    mapper["INVISIBLE"] == Token::INVISIBLE;
    mapper["KEY"] == Token::KEY;
    mapper["LINE"] == Token::LINE;
    mapper["LION"] == Token::LION;
    mapper["MONITOR"] == Token::MONITOR;
    mapper["NORMAL"] == Token::NORMAL;
    mapper["OBJECT"] == Token::OBJECT;
    mapper["PASSAGE"] == Token::PASSAGE;
    mapper["PLAYER"] == Token::PLAYER;
    mapper["PUSHER"] == Token::PUSHER;
    mapper["RICOCHET"] == Token::RICOCHET;
    mapper["RUFFIAN"] == Token::RUFFIAN;
    mapper["SCROLL"] == Token::SCROLL;
    mapper["SEGMENT"] == Token::SEGMENT;
    mapper["SHARK"] == Token::SHARK;
    mapper["SLIDEREW"] == Token::SLIDEREW;
    mapper["SLIDERNS"] == Token::SLIDERNS;
    mapper["SLIME"] == Token::SLIME;
    mapper["SOLID"] == Token::SOLID;
    mapper["SPINNINGGUN"] == Token::SPINNINGGUN;
    mapper["STAR"] == Token::STAR;
    mapper["TIGER"] == Token::TIGER;
    mapper["TRANSPORTER"] == Token::TRANSPORTER;
    mapper["WATER"] == Token::WATER;
  }

  TokenMapper::iterator iter = mapper.find(token.upper());

  if ( iter == mapper.end() ) {
    return Token::UNKNOWN;
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
    Token::Code tokenCode = tokenize( token );
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
      case Token::COUNTER: dir = cardinal_counterwise(dir); break;
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
    void addString( const ZString &line );
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

    bool parseConditional( KILLENUM &kill );
    bool parseConditionalAny( KILLENUM &kill );
    bool parseConditionalAligned( KILLENUM &kill );
    bool parseConditionalBlocked( KILLENUM &kill );

  private:
    ScriptableThing *thing;
    ProgramBank &program;
    GameWorld *world;
    GameBoard *board;
    signed short ip;
    signed short startLineIP;
    int size;
    list<ZString> tokens;
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

    if ( kill == PROCEED ) cycles --;
    else if ( kill != FREEBIE ) break;
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
  if ( !tokens.empty() ) {
    token = tokens.front();
    tokens.pop_front();
  }
  return token;
}

KILLENUM Runtime::parseNext()
{
  if ( ip >= size ) return ENDCYCLE;
  startLineIP = ip;

  const unsigned char main_symbol = program.at(ip);
  KILLENUM ret = FREEBIE;
  switch ( main_symbol )
  {
    case '@':
    case '\'':
    case ':':
      zdebug() << __FILE__ << ":" << __LINE__ << " ignored type";
      break;

    case '#':
    {
      zdebug() << __FILE__ << ":" << __LINE__ << " crunch";
      ZString line = readLine();
      tokens = line.split();
      ret = executeCrunch();
      break;
    }

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
    {
      zdebug() << __FILE__ << ":" << __LINE__ << " strings";
      ZString line = readLine();
      addString( line );
      break;
    }
  }

  // Post line cleanup
  ip = startLineIP;
  seekNextLine();

  return FREEBIE;
}

void Runtime::addString( const ZString &line )
{
  if ( line.empty() && displayLines.size() < 1 ) return;
  displayLines.push_back( line );
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
  ZString command = getToken();
  if ( command.length() <= 1 ) return throwError( "COMMAND ERROR" );

  command = command.substr(1);
  Token::Code code = tokenize(command);
  KILLENUM ret = PROCEED;

  zdebug() << __FILE__ << ":" << __LINE__ << code << command;

  switch ( code ) {
    case Token::BECOME: ret = execBecome(); break;
    case Token::BIND: ret = execBind(); break;
    case Token::CHANGE: ret = execChange(); break;
    case Token::CHAR: ret = execChar(); break;
    case Token::CLEAR: ret = execClear(); break;
    case Token::CYCLE: ret = execCycle(); break;
    case Token::DIE: ret = execDie(); break;
    case Token::END: ret = execEnd(); break;
    case Token::ENDGAME: ret = execEndgame(); break;
    case Token::GIVE: ret = execGive(); break;
    case Token::GO: ret = execGo(); break;
    case Token::IDLE: ret = execIdle(); break;
    case Token::IF: ret = execIf(); break;
    case Token::LOCK: ret = execLock(); break;
    case Token::PLAY: ret = execPlay(); break;
    case Token::PUT: ret = execPut(); break;
    case Token::RESTART: ret = execRestart(); break;
    case Token::RESTORE: ret = execRestore(); break;
    case Token::SEND: ret = execSend(); break;
    case Token::SET: ret = execSet(); break;
    case Token::SHOOT: ret = execShoot(); break;
    case Token::TAKE: ret = execTake(); break;
    case Token::THROWSTAR: ret = execThrowstar(); break;
    case Token::TRY: ret = execTry(); break;
    case Token::UNLOCK: ret = execUnlock(); break;
    case Token::WALK: ret = execWalk(); break;
    case Token::ZAP: ret = execZap(); break;
    default:
      sendMessage( command );
      break;
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
  return ENDCYCLE;
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
  KILLENUM kill = PROCEED;
  bool results = parseConditional( kill );

  if ( kill == COMMANDERROR ) return COMMANDERROR;

  if ( results ) {
    seekNextToken();
    const unsigned char symbol = program.at( ip );

    switch (symbol)
    {
      case '/': 
      case '?':
        return sugarMove();
      case '#':
        return executeCrunch();
      default:
        return throwError("CONDITIONAL ERROR");
    }
  }

  return PROCEED;
}

bool Runtime::parseConditional( KILLENUM &kill )
{
  seekNextToken();
  ZString token = getToken();
  Token::Code code = tokenize(token);

  switch ( code )
  {
    case Token::ALIGNED:
      return parseConditionalAligned(kill);

    case Token::ANY:
      return parseConditionalAny(kill);

    case Token::BLOCKED:
      return parseConditionalBlocked(kill);

    case Token::CONTACT:
      return false;

    case Token::ENERGIZED:
      return false;

    case Token::NOT:
      return !parseConditional(kill);

    default:
      // check flag
      break;
  }

  return false;
}

bool Runtime::parseConditionalAny( KILLENUM &kill )
{
  ZString token = getToken();
  Token::Code code = tokenize(token);

  // Check for color prefix first:
  Token::Code colorCode = Token::UNKNOWN;
  switch (code)
  {
    case Token::BLUE: colorCode = code; break;
    case Token::GREEN: colorCode = code; break;
    case Token::CYAN: colorCode = code; break;
    case Token::RED: colorCode = code; break;
    case Token::PURPLE: colorCode = code; break;
    case Token::YELLOW: colorCode = code; break;
    case Token::WHITE: colorCode = code; break;
    default: break;
  }

  // Get actual entity after color prefix
  if (colorCode != Token::UNKNOWN) {
    token = getToken();
    code = tokenize(token);
  }

  switch (code)
  {
    case Token::AMMO: break;
    case Token::BEAR: break;
    case Token::BLINKWALL: break;
    case Token::BOMB: break;
    case Token::BOULDER: break;
    case Token::BULLET: break;
    case Token::BREAKABLE: break;
    case Token::CLOCKWISE: break;
    case Token::COUNTER: break;
    case Token::DOOR: break;
    case Token::DUPLICATOR: break;
    case Token::EMPTY: break;
    case Token::ENERGIZER: break;
    case Token::FAKE: break;
    case Token::FOREST: break;
    case Token::GEM: break;
    case Token::HEAD: break;
    case Token::INVISIBLE: break;
    case Token::KEY: break;
    case Token::LINE: break;
    case Token::LION: break;
    case Token::MONITOR: break;
    case Token::NORMAL: break;
    case Token::OBJECT: break;
    case Token::PASSAGE: break;
    case Token::PLAYER: break;
    case Token::PUSHER: break;
    case Token::RICOCHET: break;
    case Token::RUFFIAN: break;
    case Token::SCROLL: break;
    case Token::SEGMENT: break;
    case Token::SHARK: break;
    case Token::SLIDEREW: break;
    case Token::SLIDERNS: break;
    case Token::SLIME: break;
    case Token::SOLID: break;
    case Token::SPINNINGGUN: break;
    case Token::STAR: break;
    case Token::TIGER: break;
    case Token::TORCH: break;
    case Token::TRANSPORTER: break;
    case Token::WATER: break;
    default:
      kill = throwError("CONDITIONAL ERROR");
      return false;
  }

  return false;
}

bool Runtime::parseConditionalAligned( KILLENUM &kill )
{
  ZString token = getToken();
  Token::Code code = tokenize(token);

  return false;
}

bool Runtime::parseConditionalBlocked( KILLENUM &kill )
{
  ZString token = getToken();
  Token::Code code = tokenize(token);

  return false;
}

KILLENUM Runtime::execLock()
{
  return PROCEED;
}

KILLENUM Runtime::execPlay()
{
  ZString song;
  while ( !tokens.empty() ) {
    song.append( getToken() );
  }
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
  ZString mesg = getToken();
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
  return COMMANDERROR;
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

