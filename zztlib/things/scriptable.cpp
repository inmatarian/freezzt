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
  m_interpreter->seekLabel( this, label );
}

void ScriptableThing::sendLabel( const ZString &to, const ZString &label )
{
  board()->sendLabel( to, label, this );
}

void ScriptableThing::playSong( const ZString &label )
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

