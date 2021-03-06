
#include <cassert>
#include <list>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <memory>
#include <vector>
#include <map>

#include "debug.h"
#include "defines.h"
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

// ---------------------------------------------------------------------------

namespace ZZTOOP {

// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------

namespace Token
{
  enum Code {
    UNKNOWN = 0,
    ENDOFLINE,
    // basics
    CRUNCH,
    SLASH,
    QUESTION,
    EXCLAIM,
    DOLLAR,
    AT,
    COLON,
    SINGLEQUOTE,
    NEWLINE,
    INTEGER,
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

// ---------------------------------------------------------------------------

class Tokenizer
{
  public:
    Tokenizer( const ZString &str = "" );

    void next();
    Token::Code token() const { return current; };
    ZString string() const { return word; };
    int number() const { return word.sint(); };
    bool done() const { return current == Token::ENDOFLINE; };

  protected:
    Token::Code identifySymbol( const char c ) const;

  private:
    ZString line;
    ZString word;
    Token::Code current;
    size_t left;
    size_t right;

    typedef map<ZString, Token::Code> TokenMapper;
    static TokenMapper mapper;
};

Tokenizer::TokenMapper Tokenizer::mapper;

Tokenizer::Tokenizer( const ZString &str )
  : line( str ),
    current( Token::UNKNOWN ),
    left(0),
    right(0)
{
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

  next();
}

void Tokenizer::next()
{
  // check end of line
  if ( left >= line.length() ) {
    current = Token::ENDOFLINE;
    word.clear();
    return;
  }

  // Only scan forward when not at start of line
  if ( left != 0 ) {
    left = line.find_first_not_of( tokenDelimiters, right );
    if ( left == string::npos ) {
      left = line.length();
      right = left;
      current = Token::ENDOFLINE;
      word.clear();
      return;
    }
  }

  // check for line starting symbols
  current = identifySymbol( line.at( left ) );
  if ( current != Token::UNKNOWN ) {
    word = ZString(1, line.at( left ));
    left += 1;
    right = left;
    return;
  }

  // multi-symbol token, so get the whole thing.
  right = line.find_first_of( tokenDelimiters, left );
  if ( right == string::npos ) {
    right = line.length();
  }
  word = line.substr( left, right-left );

  // check if it's a number
  if ( word.isNumber() ) {
    current = Token::INTEGER;
    return;
  }

  // identify a word
  TokenMapper::iterator iter = mapper.find(word.upper());

  if ( iter != mapper.end() ) {
    current = iter->second;
    return;
  }

  // couldn't identify it.
  current = Token::UNKNOWN;
}

Token::Code Tokenizer::identifySymbol( const char c ) const
{
  switch ( c ) {
    case '#': return Token::CRUNCH;
    case '/': return Token::SLASH;
    case '?': return Token::QUESTION;
    case '!': return Token::EXCLAIM;
    case '$': return Token::DOLLAR;
    case '@': return Token::AT;
    case ':': return Token::COLON;
    case '\'': return Token::SINGLEQUOTE;
    case zztNewLine: return Token::NEWLINE;
    default: break;
  }
  return Token::UNKNOWN;
}

// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------

unsigned char translateEntityToken( Token::Code code )
{
  switch (code)
  {
    case Token::AMMO:        return ZZTEntity::Ammo;
    case Token::BEAR:        return ZZTEntity::Bear;
    case Token::BLINKWALL:   return ZZTEntity::BlinkWall;
    case Token::BOMB:        return ZZTEntity::Bomb;
    case Token::BOULDER:     return ZZTEntity::Boulder;
    case Token::BULLET:      return ZZTEntity::Bullet;
    case Token::BREAKABLE:   return ZZTEntity::Breakable;
    case Token::CLOCKWISE:   return ZZTEntity::ClockwiseConveyer;
    case Token::COUNTER:     return ZZTEntity::CounterclockwiseConveyor;
    case Token::DOOR:        return ZZTEntity::Door;
    case Token::DUPLICATOR:  return ZZTEntity::Duplicator;
    case Token::EMPTY:       return ZZTEntity::EmptySpace;
    case Token::ENERGIZER:   return ZZTEntity::Energizer;
    case Token::FAKE:        return ZZTEntity::Fake;
    case Token::FOREST:      return ZZTEntity::Forest;
    case Token::GEM:         return ZZTEntity::Gem;
    case Token::HEAD:        return ZZTEntity::CentipedeHead;
    case Token::INVISIBLE:   return ZZTEntity::InvisibleWall;
    case Token::KEY:         return ZZTEntity::Key;
    case Token::LINE:        return ZZTEntity::Line;
    case Token::LION:        return ZZTEntity::Lion;
    case Token::MONITOR:     return ZZTEntity::Monitor;
    case Token::NORMAL:      return ZZTEntity::Normal;
    case Token::OBJECT:      return ZZTEntity::Object;
    case Token::PASSAGE:     return ZZTEntity::Passage;
    case Token::PLAYER:      return ZZTEntity::Player;
    case Token::PUSHER:      return ZZTEntity::Pusher;
    case Token::RICOCHET:    return ZZTEntity::Ricochet;
    case Token::RUFFIAN:     return ZZTEntity::Ruffian;
    case Token::SCROLL:      return ZZTEntity::Scroll;
    case Token::SEGMENT:     return ZZTEntity::CentipedeSegment;
    case Token::SHARK:       return ZZTEntity::Shark;
    case Token::SLIDEREW:    return ZZTEntity::SliderEastWest;
    case Token::SLIDERNS:    return ZZTEntity::SliderNorthSouth;
    case Token::SLIME:       return ZZTEntity::Slime;
    case Token::SOLID:       return ZZTEntity::Solid;
    case Token::SPINNINGGUN: return ZZTEntity::SpinningGun;
    case Token::STAR:        return ZZTEntity::Star;
    case Token::TIGER:       return ZZTEntity::Tiger;
    case Token::TORCH:       return ZZTEntity::Torch;
    case Token::TRANSPORTER: return ZZTEntity::Transporter;
    case Token::WATER:       return ZZTEntity::Water;
    default: break;
  }
  return ZZTEntity::NONE_ENTITY;
}

// ---------------------------------------------------------------------------

enum KILLENUM {
  FREEBIE,
  PROCEED,
  ENDCYCLE,
  COMMANDERROR
};

// ---------------------------------------------------------------------------

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

    void accept( Token::Code token = Token::UNKNOWN );
    bool expect( Token::Code token ) const;

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
    bool parseConditionalBlocked( KILLENUM &kill );

    int parseDirection( const int stackLimit = 32 );

  private:
    ScriptableThing *thing;
    ProgramBank &program;
    GameWorld *world;
    GameBoard *board;
    signed short ip;
    signed short startLineIP;
    int size;
    vector<ZString> displayLines;
    bool advanceIP;

    Tokenizer tokenizer;
};

// ---------------------------------------------------------------------------

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

void Runtime::accept( Token::Code token )
{
  if ( expect( token ) ) {
    zdebug() << "ACCEPTED TOKEN" << token;
    tokenizer.next();
  }
}

bool Runtime::expect( Token::Code token ) const
{
  return ( token == Token::UNKNOWN ) ||
         ( tokenizer.token() == token );
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
  return ZString();
}

KILLENUM Runtime::parseNext()
{
  if ( ip >= size ) return ENDCYCLE;
  startLineIP = ip;
  advanceIP = true;

  ZString line = readLine();
  tokenizer = Tokenizer(line);

  KILLENUM ret = FREEBIE;
  Token::Code main = tokenizer.token();
  switch ( main )
  {
    case Token::AT:
    case Token::SINGLEQUOTE:
    case Token::COLON:
      zdebug() << __FILE__ << ":" << __LINE__ << " ignored type";
      break;

    case Token::CRUNCH:
    {
      zdebug() << __FILE__ << ":" << __LINE__ << " crunch";
      ret = executeCrunch();
      break;
    }

    case Token::QUESTION:
    case Token::SLASH: // currently unimplemented
      zdebug() << __FILE__ << ":" << __LINE__ << " sugar move";
      ret = sugarMove();
      break;

    case Token::NEWLINE:
    case Token::EXCLAIM:
    case Token::DOLLAR:
    default:
    {
      zdebug() << __FILE__ << ":" << __LINE__ << " strings";
      addString( line );
      break;
    }
  }

  // Post line cleanup
  ip = startLineIP;
  if ( advanceIP ) {
    seekNextLine();
  }

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
  accept( Token::CRUNCH );
  Token::Code code = tokenizer.token();

  zdebug() << __FILE__ << ":" << __LINE__ << code << tokenizer.string();

  KILLENUM ret = PROCEED;
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
    case Token::ENDOFLINE: ret = throwError("COMMAND ERROR");
    default: {
      sendMessage( tokenizer.string() );
      break;
    }
  }

  return ret;
}

KILLENUM Runtime::execBecome()
{
  accept( Token::BECOME );
  return PROCEED;
}

KILLENUM Runtime::execBind()
{
  accept( Token::BIND );
  return PROCEED;
}

KILLENUM Runtime::execChange()
{
  accept( Token::CHANGE );
  return PROCEED;
}

KILLENUM Runtime::execChar()
{
  accept( Token::CHAR );
  if ( !expect( Token::INTEGER ) )
    return throwError("EXPECTED A NUMBER");

  int ch = tokenizer.number();
  accept( Token::INTEGER );

  // thing->setChar( ch );
  return PROCEED;
}

KILLENUM Runtime::execClear()
{
  accept( Token::CLEAR );
  world->removeGameFlag( tokenizer.string() );
  accept();
  return PROCEED;
}

KILLENUM Runtime::execCycle()
{
  accept( Token::CYCLE );
  if ( !expect( Token::INTEGER ) )
    return throwError("EXPECTED A NUMBER");

  int cycle = tokenizer.number();
  accept( Token::INTEGER );

  thing->setCycle( cycle );
  return PROCEED;
}

KILLENUM Runtime::execDie()
{
  accept( Token::DIE );
  return ENDCYCLE;
}

KILLENUM Runtime::execEnd()
{
  accept( Token::END );
  thing->setPaused( true );
  return ENDCYCLE;
}

KILLENUM Runtime::execEndgame()
{
  accept( Token::ENDGAME );
  return PROCEED;
}

KILLENUM Runtime::execGive()
{
  accept( Token::GIVE );
  return PROCEED;
}

KILLENUM Runtime::execGo()
{
  accept( Token::GO );
  int dir = parseDirection();
  if ( dir == DIRECTION_ERROR )
    return throwError("DIRECTION ERROR");

  const bool didMove = thing->execGo( dir );
  if ( !didMove ) advanceIP = false;

  return PROCEED;
}

KILLENUM Runtime::execIdle()
{
  accept( Token::IDLE );
  return ENDCYCLE;
}

KILLENUM Runtime::execIf()
{
  accept( Token::IF );

  KILLENUM kill = PROCEED;
  const bool results = parseConditional( kill );

  if ( kill == COMMANDERROR ) return COMMANDERROR;

  if ( results ) {
    switch ( tokenizer.token() )
    {
      case Token::SLASH:
      case Token::QUESTION:
        return sugarMove();
      case Token::CRUNCH:
        return executeCrunch();
      default:
        return throwError("CONDITIONAL ERROR");
    }
  }

  return PROCEED;
}

bool Runtime::parseConditional( KILLENUM &kill )
{
  switch ( tokenizer.token() )
  {
    case Token::ALIGNED:
      accept( Token::ALIGNED );
      return thing->alignedPlayer();

    case Token::ANY:
      return parseConditionalAny(kill);

    case Token::BLOCKED:
      return parseConditionalBlocked(kill);

    case Token::CONTACT:
      accept( Token::CONTACT );
      return thing->contactPlayer();

    case Token::ENERGIZED:
      accept( Token::ENERGIZED );
      return false;

    case Token::NOT:
      accept( Token::NOT );
      return !parseConditional(kill);

    default:
      // check flag
      break;
  }

  return false;
}

bool Runtime::parseConditionalAny( KILLENUM &kill )
{
  Token::Code code = tokenizer.token();
  Token::Code colorCode = Token::UNKNOWN;
  unsigned char colorCheck = 0xFF;

  // Check for color prefix first:
  switch (code)
  {
    case Token::BLUE:   colorCode = code; colorCheck = Defines::BLUE; break;
    case Token::GREEN:  colorCode = code; colorCheck = Defines::GREEN; break;
    case Token::CYAN:   colorCode = code; colorCheck = Defines::CYAN; break;
    case Token::RED:    colorCode = code; colorCheck = Defines::RED; break;
    case Token::PURPLE: colorCode = code; colorCheck = Defines::MAGENTA; break;
    case Token::YELLOW: colorCode = code; colorCheck = Defines::YELLOW; break;
    case Token::WHITE:  colorCode = code; colorCheck = Defines::WHITE; break;
    default: break;
  }

  // Get actual entity after color prefix
  if (colorCode != Token::UNKNOWN) {
    accept( colorCode );
    code = tokenizer.token();
  }

  // translate from Token code to ZZTEntity code.
  unsigned char idCheck = translateEntityToken( code );

  if ( idCheck == ZZTEntity::NONE_ENTITY ) {
    kill = throwError("CONDITIONAL ERROR");
    return false;
  }

  return board->isAnyEntity( idCheck, colorCheck );
}

bool Runtime::parseConditionalBlocked( KILLENUM &kill )
{
  accept( Token::BLOCKED );

  return false;
}

KILLENUM Runtime::execLock()
{
  accept( Token::LOCK );
  thing->setLocked( true );
  return PROCEED;
}

KILLENUM Runtime::execPlay()
{
  accept( Token::PLAY );

  ZString song;
  while ( tokenizer.token() != Token::ENDOFLINE ) {
    song.append( tokenizer.string() );
    accept();
  }

  thing->execPlay( song );
  return PROCEED;
}

KILLENUM Runtime::execPut()
{
  accept( Token::PUT );
  return PROCEED;
}

KILLENUM Runtime::execRestart()
{
  accept( Token::RESTART );
  sendMessage( "RESTART" );
  return PROCEED;
}

KILLENUM Runtime::execRestore()
{
  accept( Token::RESTORE );
  return PROCEED;
}

KILLENUM Runtime::execSend()
{
  accept( Token::SEND );
  sendMessage( tokenizer.string() );
  accept();
  return PROCEED;
}

KILLENUM Runtime::execSet()
{
  accept( Token::SET );
  world->addGameFlag( tokenizer.string() );
  accept();
  return PROCEED;
}

KILLENUM Runtime::execShoot()
{
  accept( Token::SHOOT );

  int dir = parseDirection();
  if ( dir == DIRECTION_ERROR )
    return throwError("DIRECTION ERROR");

  thing->execShoot( dir );

  return PROCEED;
}

KILLENUM Runtime::execTake()
{
  accept( Token::TAKE );
  return PROCEED;
}

KILLENUM Runtime::execThrowstar()
{
  accept( Token::THROWSTAR );

  int dir = parseDirection();
  if ( dir == DIRECTION_ERROR )
    return throwError("DIRECTION ERROR");

  thing->execThrowstar( dir );

  return PROCEED;
}

KILLENUM Runtime::execTry()
{
  accept( Token::TRY );

  int dir = parseDirection();
  if ( dir == DIRECTION_ERROR )
    return throwError("DIRECTION ERROR");

  const bool didMove = thing->execTry( dir );
  if ( !didMove ) {
    sendMessage( tokenizer.string() );
    return ENDCYCLE;
  }
  return PROCEED;
}

KILLENUM Runtime::execUnlock()
{
  accept( Token::UNLOCK );
  thing->setLocked( false );
  return PROCEED;
}

KILLENUM Runtime::execWalk()
{
  accept( Token::WALK );
  return PROCEED;
}

KILLENUM Runtime::execZap()
{
  accept( Token::ZAP );
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

int Runtime::parseDirection( const int stackLimit )
{
  if ( stackLimit <= 0 ) {
    return DIRECTION_ERROR;
  }

  switch ( tokenizer.token() )
  {
    case Token::IDLE:
      accept( Token::IDLE );
      return ZZTThing::Idle;

    case Token::NORTH:
      accept( Token::NORTH );
      return ZZTThing::North;

    case Token::SOUTH:
      accept( Token::SOUTH );
      return ZZTThing::South;

    case Token::WEST:
      accept( Token::WEST );
      return ZZTThing::West;

    case Token::EAST:
      accept( Token::EAST );
      return ZZTThing::East;

    case Token::SEEK:
      accept( Token::SEEK );
      return thing->seekDir();

    case Token::FLOW:
      accept( Token::FLOW );
      return thing->flowDir();

    case Token::CLOCKWISE:
      accept( Token::CLOCKWISE );
      return cardinal_clockwise( parseDirection( stackLimit - 1 ) );

    case Token::COUNTER:
      accept( Token::COUNTER );
      return cardinal_counterwise( parseDirection( stackLimit - 1 ) );

    case Token::OPPOSITE:
      accept( Token::OPPOSITE );
      return cardinal_opposite( parseDirection( stackLimit - 1 ) );

    case Token::RANDNS:
      accept( Token::RANDNS );
      return cardinal_randns();

    case Token::RANDNE:
      accept( Token::RANDNE );
      return cardinal_randne();

    case Token::RANDP:
      accept( Token::RANDP );
      return cardinal_randp( parseDirection( stackLimit - 1 ) );

    default:
      return DIRECTION_ERROR;
  }

  return DIRECTION_ERROR;
}

KILLENUM Runtime::throwError( const ZString &mesg )
{
  ip = startLineIP; // rewind to capture whole line
  ZString line = readLine();
  zinfo() << "ZZTOOP ERROR:" << mesg << "-->" << line;
  thing->setPaused(true);
  return COMMANDERROR;
}

// ---------------------------------------------------------------------------

}; //namespace

// ---------------------------------------------------------------------------

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

