/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <list>
#include <string>

#include "debug.h"
#include "zztEntity.h"
#include "gameWorld.h"
#include "gameBoard.h"
#include "abstractMusicStream.h"
#include "abstractPainter.h"

#include "zztThing.h"
#include "objects.h"

using namespace ZZTThing;
using namespace std;

ScriptableThing::ScriptableThing()
  : m_ip(0),
    m_paused(true)
{
  /* */
}

static void getOneToken( const ProgramBank &program,
                         signed short &ip,
                         string &token,
                         const string &delimiter )
{
  token.clear();
  const int size = program.size();
  while ( ip < size ) {
    const unsigned char symbol = program.at( ip );
    if ( delimiter.find(symbol) ) return;
    token.push_back( symbol );
    ip += 1;
  }
}

static void getWholeLine( const ProgramBank &program, signed short &ip, string &token )
{
  getOneToken( program, ip, token, "\n" );
}

void ScriptableThing::parseTokens( const ProgramBank &program,
                                   signed short &ip,
                                   CommandType &comType,
                                   list<string> &tokens )
{
  const int size = program.size();
  tokens.clear();
  comType = None;
  if ( ip > size ) return;

  const unsigned char main_symbol = program.at( ip++ );

  switch(main_symbol)
  {
    case '\n': return;

    case '#': {
      comType = Crunch;
      while ( program.at( ip ) != '\n' ) {
        string token;
        getOneToken( program, ip, token, " @#:/?!$'\n" );
        tokens.push_back( token );
      }
      break;
    }

    case '@': {
      comType = Name;
      string token;
      getWholeLine( program, ip, token );
      tokens.push_back( token );
      ip += 1;
      break;
    }

    case ':': {
      comType = Label;
      string token;
      getWholeLine( program, ip, token );
      tokens.push_back( token );
      ip += 1;
      break;
    }

    case '/': {
      comType = Move;
      string token;
      getOneToken( program, ip, token, " @#:/?!$'\n" );
      tokens.push_back( token );
      break;
    }

    case '?': {
      comType = Try;
      string token;
      getOneToken( program, ip, token, " @#:/?!$'\n" );
      tokens.push_back( token );
      break;
    }

    case '!': {
      comType = Menu;
      string token;
      getOneToken( program, ip, token, " :\n" );
      tokens.push_back( token );
      if ( program.at( ip ) == '\n' ) {
        ip += 1;
        break;
      }
      getWholeLine( program, ip, token );
      tokens.push_back( token );
      break;
    }

    case '$': {
      comType = Text;
      string token;
      getWholeLine( program, ip, token );
      tokens.push_back( token );
      ip += 1;
      break;
    }

    case '\'': {
      comType = Remark;
      string token;
      getWholeLine( program, ip, token );
      tokens.push_back( token );
      ip += 1;
      break;
    }

    case ' ':
    default: {
      comType = Text;
      string token;
      getWholeLine( program, ip, token );
      tokens.push_back( token );
      ip += 1;
      break;
    }
  }
}

// -------------------------------------

Scroll::Scroll()
{
  setPaused( true );
}

void Scroll::exec_impl() { /* */ };

void Scroll::handleTouched()
{
  zdebug() << "Scroll::handleTouched";
  doDie();
}

// -------------------------------------

Object::Object()
{
  setPaused( false );
}

void Object::exec_impl() { /* */ };

void Object::handleTouched()
{
  zdebug() << "Object::handleTouched";
}

