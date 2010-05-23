/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef __ZZT_THING_OBJECTS_H__
#define __ZZT_THING_OBJECTS_H__

#include <vector>
#include <list>
#include <string>

#include "zztThing.h"
#include "scriptable.h"

// =================

namespace ZZTThing {

// =================

class Scroll : public ScriptableThing
{
  public:
    Scroll();
    virtual unsigned char entityID() const { return ZZTEntity::Scroll; };
    virtual unsigned char tile() const { return 0xE8; };

    virtual void handleTouched();

 protected:
    virtual void exec_impl();
};

// -------------------------------------

class Object : public ScriptableThing
{
  public:
    Object();
    virtual unsigned char entityID() const { return ZZTEntity::Object; };
    virtual unsigned char tile() const { return m_char; };

    virtual void setCharacter( unsigned char ch ) { m_char = ch; };

    virtual void handleTouched();

  protected:
    virtual void exec_impl();

  private:
    unsigned char m_char;
};

// =================

} // namespace

// =================

#endif /* __ZZT_THING_OBJECTS_H__ */

