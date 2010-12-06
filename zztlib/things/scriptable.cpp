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
    m_paused(true),
    m_locked(false)
{
  /* */
}

inline bool verifyTokens( const std::list<ZString> &tokens,
                          unsigned int min, unsigned int max = 0 )
{
  if ( (max > 0) && (tokens.size()>max) ) return false;
  return ( tokens.size() >= min );
}

void ScriptableThing::setInterpreter( ZZTOOP::Interpreter *interp )
{
  m_interpreter = interp;
  ZString name = interp->getObjectName();
  setObjectName( name );
}

void ScriptableThing::setObjectName( const ZString &name )
{
  zinfo() << "Object named:" << name;
  m_name = name;
}

void ScriptableThing::setInstructionPointer( signed short ip )
{
  m_ip = (unsigned int) ip;
  // TODO: Handle objects with an ip of -1 (binding another robot)
}

void ScriptableThing::run( int cycles )
{
  m_interpreter->run( this, cycles );
}

void ScriptableThing::seekLabel( const ZString &label )
{
  execSend( label );
}

void ScriptableThing::execSend( const ZString &label )
{
  m_interpreter->seekLabel( this, label );
}

void ScriptableThing::execSend( const ZString &to, const ZString &label )
{
  board()->sendLabel( to, label, this );
}

void ScriptableThing::execPlay( const ZString &label )
{
  musicStream()->playMusic( label.c_str() );
}

void ScriptableThing::showMessage( const ZString &mesg )
{
  board()->setMessage( mesg );
}

void ScriptableThing::showScroll( TextScrollModel *model )
{
  world()->scrollView()->setModel( model );
}

void ScriptableThing::execSet( const ZString &flag )
{
  world()->addGameFlag( flag );
}

void ScriptableThing::execClear( const ZString &flag )
{
  world()->removeGameFlag( flag );
}

void ScriptableThing::execZap( const ZString &label )
{
  // Deliberate bug. Shared Interpreters are affected by Zap
  m_interpreter->zapLabel( label );
}

void ScriptableThing::execRestore( const ZString &label )
{
  // Deliberate bug. Shared Interpreters are affected by Restore
  m_interpreter->restoreLabel( label );
}

void ScriptableThing::execBecome( unsigned char id, unsigned char color )
{ /* */ }

void ScriptableThing::execBind( const ZString &name )
{ /* */ }

void ScriptableThing::execChange( unsigned char frId, unsigned char frColor,
                 unsigned char toId, unsigned char toColor )
{ /* */ }

void ScriptableThing::execChar( unsigned char ch )
{ /* */ }

void ScriptableThing::execCycle( int number )
{ /* */ }

void ScriptableThing::execDie()
{
  setPaused( true );
}

void ScriptableThing::execEnd()
{
  setPaused( true );
}

void ScriptableThing::execEndgame()
{ /* */ }

void ScriptableThing::execGive( int id, int amount )
{ /* */ }

bool ScriptableThing::execGo( int dir )
{
  return true;
}

void ScriptableThing::execIdle()
{ /* */ }

void ScriptableThing::execIf( const ZString &flag, const ZString &message )
{ /* */ }

void ScriptableThing::execLock()
{ /* */ }

void ScriptableThing::execPut( int dir, unsigned char id, unsigned char color )
{ /* */ }

void ScriptableThing::execRestart()
{ /* */ }

void ScriptableThing::execShoot( int dir )
{ /* */ }

void ScriptableThing::execTake( int item, int amount, const ZString &message )
{ /* */ }

void ScriptableThing::execThrowstar( int dir )
{ /* */ }

bool ScriptableThing::execTry( int dir )
{
  return false;
}

void ScriptableThing::execUnlock()
{ /* */ }

void ScriptableThing::execWalk( int dir )
{ /* */ }

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
 : m_char( 0x01 )
{
  setPaused( false );
}

void Object::exec_impl()
{
  ScriptableThing::run(33);
}

void Object::handleTouched()
{
  zdebug() << "Object::handleTouched";
  seekLabel( "touch" );
}

