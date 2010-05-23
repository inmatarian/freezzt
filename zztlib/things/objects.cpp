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
#include "zztEntity.h"
#include "gameWorld.h"
#include "gameBoard.h"
#include "abstractMusicStream.h"
#include "abstractPainter.h"

#include "zztThing.h"
#include "scriptable.h"
#include "objects.h"

using namespace ZZTThing;

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

